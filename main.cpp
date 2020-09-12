#include <iostream>
#include <fstream>
#include <limits.h>
#include <time.h>
#include <cfloat>

#include "ray.h"
#include "sphere.h"
#include "moving_sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "constant_texture.h"
#include "checker_texture.h"
#include "diffuse_light.h"
#include "rect.h"
#include "box.h"
#include "flip_normals.h"
#include "translate.h"
#include "rotate.h"

using namespace std;

const string file("cornell.pgm");

vec3 color (const ray& r, hitable *world, int depth) {
	hit_record rec;
	
	if (world->hit(r, 0.001, FLT_MAX, rec)) {
		ray scattered;
		vec3 attenuation;
		vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
			return emitted + attenuation*color(scattered, world, depth+1);
		}
		else {
			return emitted;
		}
	}
	else {
		/*
		//global ilumination
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5*(unit_direction.y()+1.0);
		return (1.0-t)*vec3(1.0,1.0,1.0) + t*vec3(0.5,0.7,1.0);
		*/
		//black background
		return vec3 (0.0, 0.0, 0.0);
	}
}

hitable *random_scene() {
    int n = 500;
    hitable **list = new hitable*[n+1];
	texture *checker = new checker_texture ( new constant_texture (vec3 (0.2,0.3,0.1)), new constant_texture(vec3 (0.9,0.9,0.9)));
    //list[0] =  new sphere(vec3(0,-1000,0), 1000, new lambertian(checker));
    list[0] =  new sphere(vec3(0,-1000,0), 1000, new diffuse_light(new constant_texture (vec3(40,40,40))));
    int i = 1;
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            float choose_mat = float(rand())/RAND_MAX;
//			vec3 center(a+0.9*drand48(), 0.2,b+0.9*drand48());
            vec3 center(a+0.9*float(rand())/RAND_MAX,0.2,b+0.9*float(rand())/RAND_MAX);
            if ((center-vec3(4,0.2,0)).length() > 0.9) {
                if (choose_mat < 0.8) {  // diffuse
                    list[i++] = new moving_sphere(
                        center, center + vec3(0,0.5*drand48(), 0), 0.0, 1.0, 0.2,
                        new lambertian(new constant_texture(vec3(float(rand())/RAND_MAX *float(rand())/RAND_MAX,
                                            float(rand())/RAND_MAX *float(rand())/RAND_MAX,
                                            float(rand())/RAND_MAX *float(rand())/RAND_MAX)))
                    );
                }
                else if (choose_mat < 0.95) { // metal
                    list[i++] = new sphere(
                        center, 0.2,
                        new metal(vec3(0.5*(1 + float(rand())/RAND_MAX),
                                       0.5*(1 + float(rand())/RAND_MAX),
                                       0.5*(1 + float(rand())/RAND_MAX)),
                                  0.5*float(rand())/RAND_MAX)
                    );
                }
                else {  // glass
                    list[i++] = new sphere(center, 0.2, new dielectric(1.5));
                }
            }
        }
    }

    list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1))));
    list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));

    return new hitable_list(list,i);
}

hitable *simple_light () {
	texture *text = new constant_texture (vec3 (1, 1, 1));
	hitable **list = new hitable*[4];
	list[0] = new sphere (vec3(0,-1000,0), 1000, new lambertian(text));
	list[1] = new sphere (vec3(0, 2, 0), 2, new lambertian(new constant_texture (vec3 (0,0,1))));
	list[2] = new sphere (vec3(0, 7, 0), 2, new diffuse_light( new constant_texture (vec3(4,4,4))));
	list[3] = new xy_rect (3, 5, 1, 3, -2, new diffuse_light (new constant_texture (vec3(4,4,4))));

	return new hitable_list(list, 4);
}

hitable *cornell_box () {
	hitable **list = new hitable*[8];
	int i = 0;
	material *red = new lambertian (new constant_texture(vec3(0.65, 0.05, 0.05)));
	material *white = new lambertian (new constant_texture (vec3 (0.73, 0.73, 0.73)));
	material *green = new lambertian (new constant_texture (vec3 (0.12, 0.45, 0.15)));
	material *light = new diffuse_light (new constant_texture (vec3(15, 15, 15)));
	list[i++] = new flip_normals ( new yz_rect(0, 555, 0, 555, 555, green) );
	list[i++] = new yz_rect(0, 555, 0, 555, 0, red);
	list[i++] = new xz_rect(213, 343, 227, 332, 554, light);
	list[i++] = new flip_normals (new xz_rect(0, 555, 0, 555, 555, white));
	list[i++] = new xz_rect(0, 555, 0, 555, 0, white);
	list[i++] = new flip_normals ( new xy_rect(0, 555, 0, 555, 555, white) );

	list[i++] = new translate(new rotate_y ( new box (vec3(0, 0, 0), vec3(165, 165, 165), white), -18), vec3(130, 0,  65));
	list[i++] = new translate(new rotate_y ( new box (vec3(0, 0, 0), vec3(165, 330, 165), white),  15), vec3(265, 0, 295));

	return new hitable_list (list, i);
}

hitable *snow_man_scene() {
	hitable **list = new hitable*[20];

	material *white = new lambertian (new constant_texture(vec3(0.73, 0.73, 0.73)));
	material *black = new lambertian (new constant_texture(vec3(0, 0, 0)));
	material *orange = new lambertian (new constant_texture(vec3(1, 0.5, 0)));

	//ground
    list[0] = new sphere(vec3(0,-1000,0), 1000, white);

	//body
	list[1] = new sphere(vec3(0,  4, 0), 8, white);
	list[2] = new sphere(vec3(0, 13, 0), 5, white);
	list[3] = new sphere(vec3(0, 19, 0), 3, white);

	//buttons
	list[4] = new sphere(vec3(0, 12.7, 4.5), 1, black);
	list[5] = new sphere(vec3(0, 15.4, 4.5), 0.9, black);

	//eyes
	list[6] = new sphere(vec3( 1, 20, 4.5), 0.4, black);
	list[7] = new sphere(vec3(-1, 20, 4.5), 0.4, black);

	//mouth
	list[8] = new sphere(vec3( 0.6, 18, 4.5), 0.4, black);
	list[9] = new sphere(vec3(-0.6, 18, 4.5), 0.4, black);
	list[10] = new sphere(vec3( 0.6 + 0.83, 18.3, 4.5), 0.4, black);
	list[11] = new sphere(vec3(-0.6 - 0.83, 18.3, 4.5), 0.4, black);

	//arm
	list[12] = new rotate_z (new translate (new xy_rect(0, 14, 0, 0.5, 0, black), vec3(-2, 13, 0)), -45);
	list[13] = new translate (new rotate_z (new xy_rect(0, 14, 0, 0.5, 0, black), 155), vec3(10, 18, 0));

	return new hitable_list (list, 14);
}

int main () {
	ofstream outf(file);

	srand(time(NULL));

	int nx = 800, ny = 600, ns = 10;
	outf << "P3" << endl << nx << " " << ny << endl << "255" << endl;

	//	hitable *world = random_scene();
	//hitable *world = simple_light();
	hitable *world = cornell_box();
	//hitable *world = snow_man_scene();
	//camera for the cornell box

	vec3 lookfrom (278,278,-800);
	vec3 lookat (278,278,0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);

	//camera for the snowman
	/*
	vec3 lookfrom (0, 16, 46);
	vec3 lookat (0,8,0);
	float dist_to_focus = 10.0;
	float aperture = 0.0;
	float vfov = 40.0;
	camera cam(lookfrom, lookat, vec3(0,1,0), vfov, float(nx)/float(ny), aperture, dist_to_focus, 0.0, 1.0);
	*/

	for (int j = ny-1 ; j >= 0 ; j--) {
		for (int i = 0 ; i < nx ; i++) {

			vec3 col (0.0,0.0,0.0);

			for (int s = 0 ; s < ns ; s++) {
				float u = float(i+float(rand())/float(RAND_MAX))/float(nx);
				float v = float(j+float(rand())/float(RAND_MAX))/float(ny);

				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at_parameter(2.0);
				col += color (r, world, 0);
			}

			col /= float(ns);

			col = vec3 (sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));

			int ir = int(255.99f*col[0]);
			int ig = int(255.99f*col[1]);
			int ib = int(255.99f*col[2]);

			outf << ir << " " << ig << " " << ib << endl;

		}
	}

	return 0;
}
