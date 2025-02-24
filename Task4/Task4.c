#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lodepng.h"

#define KERNEL_SIZE 3
#define KERNEL_SUM 16 // Sum of Gaussian kernel weights
#define DEFAULT_THREADS 4

// Gaussian Kernel (3x3)
const int kernel[3][3] = {
    {1, 2, 1},
    {2, 4, 2},
    {1, 2, 1}};

unsigned char *image, *output_image;
int width, height;

// Struct for thread arguments
struct ThreadData
{
    int start_row, end_row;
};

void *apply_gaussian_blur(void *arg)
{
    struct ThreadData *data = (struct ThreadData *)arg;
    int start = data->start_row;
    int end = data->end_row;

    for (int y = start; y < end; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int sum_red = 0, sum_green = 0, sum_blue = 0;
            int weight_sum = 0;

            // Apply 3x3 Gaussian Kernel
            for (int ky = -1; ky <= 1; ky++)
            {
                for (int kx = -1; kx <= 1; kx++)
                {
                    int pixel_x = x + kx;
                    int pixel_y = y + ky;

                    // Check bounds
                    if (pixel_x >= 0 && pixel_x < width && pixel_y >= 0 && pixel_y < height)
                    {
                        int pixel_index = 4 * (pixel_y * width + pixel_x);
                        int weight = kernel[ky + 1][kx + 1];

                        sum_red += image[pixel_index] * weight;
                        sum_green += image[pixel_index + 1] * weight;
                        sum_blue += image[pixel_index + 2] * weight;
                        weight_sum += weight;
                    }
                }
            }

            // Store blurred pixel
            int new_index = 4 * (y * width + x);
            output_image[new_index] = sum_red / weight_sum;
            output_image[new_index + 1] = sum_green / weight_sum;
            output_image[new_index + 2] = sum_blue / weight_sum;
            output_image[new_index + 3] = image[new_index + 3]; // Preserve alpha
        }
    }
    pthread_exit(NULL);
}

int main()
{
    char filename[] = "Flower.png", output_filename[] = "Blurred.png";
    int num_threads = DEFAULT_THREADS;

    printf("Loading image: %s\n", filename);
    int error = lodepng_decode32_file(&image, &width, &height, filename);
    if (error)
    {
        printf("Error loading image! Code: %d - %s\n", error, lodepng_error_text(error));
        return 1;
    }
    printf("Image loaded successfully: Width = %d, Height = %d\n", width, height);

    output_image = (unsigned char *)malloc(width * height * 4);
    if (!output_image)
    {
        printf("Memory allocation failed!\n");
        free(image);
        return 1;
    }

    if (num_threads > height)
        num_threads = height;

    pthread_t threads[num_threads];
    struct ThreadData thread_data[num_threads];
    int rows_per_thread = height / num_threads;
    int extra_rows = height % num_threads;

    int start_row = 0;
    for (int i = 0; i < num_threads; i++)
    {
        int end_row = start_row + rows_per_thread + (i < extra_rows ? 1 : 0);
        thread_data[i].start_row = start_row;
        thread_data[i].end_row = end_row;
        pthread_create(&threads[i], NULL, apply_gaussian_blur, &thread_data[i]);
        start_row = end_row;
    }

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }

    printf("Saving output image as %s...\n", output_filename);
    int save_error = lodepng_encode32_file(output_filename, output_image, width, height);
    if (save_error)
    {
        printf("Error saving image! Code: %d - %s\n", save_error, lodepng_error_text(save_error));
    }
    else
    {
        printf("Blurred image saved successfully as %s!\n", output_filename);
    }

    free(image);
    free(output_image);
    return 0;
}
