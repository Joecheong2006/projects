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

static void resolve_velocity(collision2d_state* state, rigid2d* r1, rigid2d* r2) {
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

static void resolve_rotation(collision2d_state* state, rigid2d* r1, rigid2d* r2) {
    vec2 contact1, contact2;
    glm_vec2_sub(state->contact, r1->tran->position, contact1);
    glm_vec2_sub(state->contact, r2->tran->position, contact2);
    vec2 contact_perp1 = {-contact1[1], contact1[0]};
    vec2 contact_perp2 = {-contact2[1], contact2[0]};

    draw_debug_circle((vec3){state->contact[0], state->contact[1], 0}, 0.1, (vec3){1, 1, 0});

    vec2 av1 = {contact_perp1[0] * r1->angular_v, contact_perp1[1] * r1->angular_v};
    vec2 av2 = {contact_perp2[0] * r2->angular_v, contact_perp2[1] * r2->angular_v};
    vec2 relative_a = {
        r1->v[0] + av1[0] - r2->v[0] - av2[0],
        r1->v[1] + av1[1] - r2->v[1] - av2[1],
    };

    const f32 relative_d = glm_vec2_dot(relative_a, state->normal);
    if (relative_d < 0) {
        return;
    }

    const f32 e = glm_min(r1->restitution, r2->restitution);
    const f32 contact_count = 2;
    f32 perp1_dot_n = glm_vec2_dot(contact_perp1, state->normal);
    f32 perp2_dot_n = glm_vec2_dot(contact_perp2, state->normal);
    f32 p1di = perp1_dot_n * perp1_dot_n * r1->inverse_inertia;
    f32 p2di = perp2_dot_n * perp2_dot_n * r2->inverse_inertia;
    const f32 J = -relative_d * (e + 1.0) / (p1di + p2di + r1->inverse_mass + r2->inverse_mass) / contact_count;
    vec2 impulse = { J * state->normal[0], J * state->normal[1] };

    r1->angular_v += glm_vec2_cross(contact1, impulse) * r1->inverse_inertia;
    r2->angular_v -= glm_vec2_cross(contact2, impulse) * r2->inverse_inertia;
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
	for_vector(instance.objects, i, 0) {
		rigid2d* body = instance.objects[i];
		body->process(body, 1.0 / 144);
	}

	for_vector(instance.objects, i, 0) {
		rigid2d* body1 = instance.objects[i];
		for_vector(instance.objects, j, 0) {
			rigid2d* body2 = instance.objects[j];
			if (body1 == body2) {
				break;
			}
			if (body1->collider == NULL || body2->collider == NULL) {
				continue;
			}
			if (body1->is_static && body2->is_static) {
				continue;
			}
			collision2d_state state = body1->collider->collide(body1->collider, body2->collider);
			if (state.depth > 0) {
				resolve_rotation(&state, body1->collider->parent, body2->collider->parent);
				resolve_velocity(&state, body1->collider->parent, body2->collider->parent);
			}
		}
	}
}

void shutdown_physics2d_object_system() {
	free_vector(instance.objects);
}

