#include "physics2d_object_system.h"
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

static void resolve_rotation(vec2 out_c1, vec2 out_c2, vec2 out_impulse, collision2d_point* state, rigid2d* r1, rigid2d* r2) {
    vec2 contact1, contact2;
    glm_vec2_sub(state->contact, r1->tran->position, contact1);
    glm_vec2_sub(state->contact, r2->tran->position, contact2);
    vec2 contact_perp1 = {-contact1[1], contact1[0]};
    vec2 contact_perp2 = {-contact2[1], contact2[0]};

    glm_vec2_copy(contact1, out_c1);
    glm_vec2_copy(contact2, out_c2);

    draw_debug_circle((vec3){state->contact[0], state->contact[1], 0}, 0.1, (vec3){1, 1, 0});

    vec2 av1 = {contact_perp1[0] * r1->angular_v, contact_perp1[1] * r1->angular_v};
    vec2 av2 = {contact_perp2[0] * r2->angular_v, contact_perp2[1] * r2->angular_v};
    vec2 relative_a = {
        r1->v[0] + av1[0] - r2->v[0] - av2[0],
        r1->v[1] + av1[1] - r2->v[1] - av2[1],
    };

    const f32 relative_d = glm_vec2_dot(relative_a, state->normal);
    if (relative_d < 0) {
    	glm_vec2_zero(out_impulse);
        return;
    }

    const f32 e = glm_min(r1->restitution, r2->restitution);
    f32 perp1_dot_n = glm_vec2_dot(contact_perp1, state->normal);
    f32 perp2_dot_n = glm_vec2_dot(contact_perp2, state->normal);
    f32 p1di = perp1_dot_n * perp1_dot_n * r1->inverse_inertia;
    f32 p2di = perp2_dot_n * perp2_dot_n * r2->inverse_inertia;
    const f32 J = -relative_d * (e + 1.0) / (p1di + p2di + r1->inverse_mass + r2->inverse_mass);
    vec2 impulse = { J * state->normal[0], J * state->normal[1] };

    out_impulse[0] = impulse[0];
    out_impulse[1] = impulse[1];
}

void destory_physics2d_object(rigid2d* body) {
	i32 len = vector_size(instance.objects);
	instance.objects[body->index] = instance.objects[len - 1];
	vector_pop(instance.objects);
}

void setup_physics2d_object_system() {
	instance.objects = make_vector();
}

static void update_collision() {
	for_vector(instance.objects, i, 0) {
		rigid2d* body1 = instance.objects[i];
		for_vector(instance.objects, j, 0) {
			rigid2d* body2 = instance.objects[j];
			if (body1 == body2) {
				continue;
				// break;
			}
			if (body1->collider == NULL || body2->collider == NULL) {
				continue;
			}
			if (body1->is_static && body2->is_static) {
				continue;
			}
			collision2d_info info = body1->collider->collide(body1->collider, body2->collider);
			vec2 rimpulse[info.points_count];
			vec2 c1[info.points_count];
			vec2 c2[info.points_count];
			int actual_contact_count = 0;
			for (int i = 0; i < info.points_count; ++i) {
				if (info.collision_points[i].depth >= 0) {
					resolve_penetration(&info.collision_points[i], body1, body2);
					resolve_rotation(c1[i], c2[i], rimpulse[i], &info.collision_points[i], body1, body2);
					actual_contact_count++;
				}
			}
			for (int i = 0; i < actual_contact_count; ++i) {
				body1->angular_v += glm_vec2_cross(c1[i], rimpulse[i]) * body1->inverse_inertia / actual_contact_count;
				body2->angular_v -= glm_vec2_cross(c2[i], rimpulse[i]) * body2->inverse_inertia / actual_contact_count;
			    glm_vec2_muladds(rimpulse[i], body1->inverse_mass, body1->v);
			    glm_vec2_mulsubs(rimpulse[i], body2->inverse_mass, body2->v);
			}
		}
	}
}

void update_physics2d_object_system() {
	const f32 step_count = 4, dt = 1.0 / 144;
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

