#include "physics2d_object_system.h"
#include "core/assert.h"
#include "physics2d/collider2d.h"
#include "basic/vector.h"

#include "debug/primitive_shape_renderer.h"

static struct physics2d_object_system {
	vector(rigid2d*) objects;
} instance;

physics2d_object_system* get_physics2d_object_system() {
	return &instance;
}
void create_physics2d_object(rigid2d* body) {
	ASSERT(body != NULL);
	body->index = vector_size(instance.objects);
	vector_push(instance.objects, body);
}

static void resolve_penetration(collision2d_point* state, rigid2d* r1, rigid2d* r2) {
    const f32 total_inverse_mass = 1.0 / (r1->inverse_mass + r2->inverse_mass);
    if (r2->is_static) {
        glm_vec2_mulsubs(state->normal, state->depth, r1->tran->position);
    }
    else if (r1->is_static) {
        glm_vec2_muladds(state->normal, state->depth, r2->tran->position);
    }
    else {
        glm_vec2_mulsubs(state->normal, state->depth * r1->inverse_mass * total_inverse_mass, r1->tran->position);
        glm_vec2_muladds(state->normal, state->depth * r2->inverse_mass * total_inverse_mass, r2->tran->position);
    }
}

static void resolve_velocity(collision2d_point* state, rigid2d* r1, rigid2d* r2) {
    const f32 total_inverse_mass = 1.0 / (r1->inverse_mass + r2->inverse_mass);

    vec2 separate_v;
    glm_vec2_sub(r1->v, r2->v, separate_v);
    const f32 relative_normal = glm_vec2_dot(separate_v, state->normal);
    if (relative_normal < 0) {
        return;
    }

    const f32 e = glm_min(r1->restitution, r2->restitution);
    const f32 J = -relative_normal * (e + 1.0) * total_inverse_mass;

    glm_vec2_muladds(state->normal, J * r1->inverse_mass, r1->v);
    glm_vec2_mulsubs(state->normal, J * r2->inverse_mass, r2->v);
}

static void resolve_rotation(f32* out_j, vec2 out_c1, vec2 out_c2, vec2 out_impulse, f32 inverse_count, collision2d_point* state, rigid2d* r1, rigid2d* r2) {
    vec2 contact1, contact2;
    glm_vec2_sub(state->contact, r1->tran->position, contact1);
    glm_vec2_sub(state->contact, r2->tran->position, contact2);
    vec2 contact_perp1 = {-contact1[1], contact1[0]};
    vec2 contact_perp2 = {-contact2[1], contact2[0]};

    glm_vec2_copy(contact1, out_c1);
    glm_vec2_copy(contact2, out_c2);

    vec2 av1 = {contact_perp1[0] * r1->angular_v, contact_perp1[1] * r1->angular_v};
    vec2 av2 = {contact_perp2[0] * r2->angular_v, contact_perp2[1] * r2->angular_v};
    vec2 relative_a = {
        r1->v[0] + av1[0] - r2->v[0] - av2[0],
        r1->v[1] + av1[1] - r2->v[1] - av2[1],
    };

    const f32 relative_d = glm_vec2_dot(relative_a, state->normal);
    if (relative_d < 0) {
    	*out_j = 0;
    	glm_vec2_zero(out_impulse);
        return;
    }

    const f32 e = glm_min(r1->restitution, r2->restitution);
    const f32 perp1_dot_n = glm_vec2_dot(contact_perp1, state->normal);
    const f32 perp2_dot_n = glm_vec2_dot(contact_perp2, state->normal);
    const f32 p1di = perp1_dot_n * perp1_dot_n * r1->inverse_inertia;
    const f32 p2di = perp2_dot_n * perp2_dot_n * r2->inverse_inertia;
    const f32 J = -inverse_count* relative_d * (e + 1.0) / (p1di + p2di + r1->inverse_mass + r2->inverse_mass);
    *out_j = J;
    vec2 impulse = { J * state->normal[0], J * state->normal[1] };

    out_impulse[0] = impulse[0];
    out_impulse[1] = impulse[1];
}

static void resolve_friction(f32 in_j, f32 inverse_count, vec2 out_impulse, collision2d_point* state, rigid2d* r1, rigid2d* r2) {
    vec2 contact1, contact2;
    glm_vec2_sub(state->contact, r1->tran->position, contact1);
    glm_vec2_sub(state->contact, r2->tran->position, contact2);
    vec2 contact_perp1 = {-contact1[1], contact1[0]};
    vec2 contact_perp2 = {-contact2[1], contact2[0]};

    vec2 av1 = {contact_perp1[0] * r1->angular_v, contact_perp1[1] * r1->angular_v};
    vec2 av2 = {contact_perp2[0] * r2->angular_v, contact_perp2[1] * r2->angular_v};
    vec2 relative_a = {
        r1->v[0] + av1[0] - r2->v[0] - av2[0],
        r1->v[1] + av1[1] - r2->v[1] - av2[1],
    };

    f32 rdn = glm_vec2_dot(relative_a, state->normal);
    vec2 tangent = {
    	relative_a[0] - state->normal[0] * rdn,
    	relative_a[1] - state->normal[1] * rdn,
    };

    if (fabs(tangent[0]) < 0.001 && fabs(tangent[1]) < 0.001) {
    	glm_vec2_zero(out_impulse);
    	return;
    }
	glm_vec2_normalize(tangent);

    const f32 perp1_dot_t = glm_vec2_dot(contact_perp1, tangent);
    const f32 perp2_dot_t = glm_vec2_dot(contact_perp2, tangent);
    const f32 p1di = perp1_dot_t * perp1_dot_t * r1->inverse_inertia;
    const f32 p2di = perp2_dot_t * perp2_dot_t * r2->inverse_inertia;
    const f32 J = -inverse_count * glm_vec2_dot(relative_a, tangent) / (p1di + p2di + r1->inverse_mass + r2->inverse_mass);

    vec2 impulse;
    
    const f32 static_friction = 0.2;
    const f32 dynamic_friction = 0.1;

    if (fabs(J) < in_j * static_friction) {
    	impulse[0] = -J * tangent[0];
    	impulse[1] = -J * tangent[1];
    }
    else {
    	impulse[0] = in_j * tangent[0] * dynamic_friction;
    	impulse[1] = in_j * tangent[1] * dynamic_friction;
    }

    out_impulse[0] = impulse[0];
    out_impulse[1] = impulse[1];
}

static void resolve_collision(i32* collision_point_index, i32 collision_count, collision2d_info* info, rigid2d* r1, rigid2d* r2) {
	vec2 rimpulse[info->points_count];
	vec2 fimpulse[info->points_count];
	f32 Js[info->points_count];
	vec2 c1[info->points_count];
	vec2 c2[info->points_count];

	f32 inverse_count = 1.0 / collision_count;

	for (i32 i = 0; i < collision_count; ++i) {
		i32 index = collision_point_index[i];
		resolve_penetration(info->collision_points + index, r1, r2);
		resolve_rotation(Js + i, c1[i], c2[i], rimpulse[i], inverse_count, info->collision_points + index, r1, r2);
	}

	for (i32 i = 0; i < collision_count; ++i) {
		r1->angular_v += glm_vec2_cross(c1[i], rimpulse[i]) * r1->inverse_inertia;
		r2->angular_v -= glm_vec2_cross(c2[i], rimpulse[i]) * r2->inverse_inertia;
	    glm_vec2_muladds(rimpulse[i], r1->inverse_mass, r1->v);
	    glm_vec2_mulsubs(rimpulse[i], r2->inverse_mass, r2->v);
	}

	for (i32 i = 0; i < collision_count; ++i) {
		i32 index = collision_point_index[i];
		resolve_friction(Js[i], inverse_count, fimpulse[i], info->collision_points + index, r1, r2);
	}

	for (i32 i = 0; i < collision_count; ++i) {
		r1->angular_v += glm_vec2_cross(c1[i], fimpulse[i]) * r1->inverse_inertia;
		r2->angular_v -= glm_vec2_cross(c2[i], fimpulse[i]) * r2->inverse_inertia;
	    glm_vec2_muladds(fimpulse[i], r1->inverse_mass, r1->v);
	    glm_vec2_mulsubs(fimpulse[i], r2->inverse_mass, r2->v);
	}
}

static void update_collision() {
	for_vector(instance.objects, i, 0) {
		rigid2d* body1 = instance.objects[i];
		if (!body1->collider) {
			continue;
		}
		for_vector(instance.objects, j, 0) {
			rigid2d* body2 = instance.objects[j];
			if (body1 == body2) {
				continue;
				// break;
			}
			if (body2->collider == NULL) {
				continue;
			}
			if (body1->is_static && body2->is_static) {
				continue;
			}

			collision2d_info info = body1->collider->collide(body1->collider, body2->collider);
			i32 index[info.points_count], count = 0;
			for (int i = 0; i < info.points_count; ++i) {
				if (info.collision_points[i].depth > 0) {
					index[count++] = i;
				}
			}

			if (count > 0) {
				resolve_collision(index, count, &info, body1, body2);
			}
		}
	}
}

void destory_physics2d_object(rigid2d* body) {
	i32 len = vector_size(instance.objects);
	instance.objects[body->index] = instance.objects[len - 1];
	vector_pop(instance.objects);
}

void setup_physics2d_object_system() {
	instance.objects = make_vector();
}

void update_physics2d_object_system() {
	const f32 step_count = 2, dt = 1.0 / 144;
	for (int i = 0; i < step_count; i++) {
		for_vector(instance.objects, i, 0) {
			rigid2d* body = instance.objects[i];
			body->process(body, dt / step_count);
		}
		update_collision();
	}
}

void shutdown_physics2d_object_system() {
	free_vector(instance.objects);
}

