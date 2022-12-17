#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include "jansson/jansson.h"


void print_json(json_t *root);
void print_json_aux(char *parent_key, char*key, json_t *element, int indent);
void print_json_indent(int indent);
const char *json_plural(size_t count);
void print_json_object(char *parent_key, json_t *element, int indent);
void print_json_array(char *parent_key, char *key, json_t *element, int indent);
void print_json_string(json_t *element, int indent);
void print_json_integer(json_t *element, int indent);
void print_json_real(json_t *element, int indent);
void print_json_true(json_t *element, int indent);
void print_json_false(json_t *element, int indent);
void print_json_null(json_t *element, int indent);

void print_json(json_t *root) { print_json_aux("", "", root, 0); }

void print_json_aux(char *parent_key, char *key, json_t *element, int indent) {
    switch (json_typeof(element)) {
        case JSON_OBJECT:
            print_json_object(key, element, indent);
            break;
        case JSON_ARRAY:
            print_json_array(parent_key, key, element, indent);
            break;
        default:
            if(!strcmp(parent_key, "current_condition") || !strcmp(parent_key, "areaName") || !strcmp(parent_key, "country") || !strcmp(parent_key, "astronomy") || !strcmp(parent_key, "lang_ru"))
                if (!strcmp(key, "FeelsLikeC") || !strcmp(key, "pressure") || !strcmp(key, "temp_C") || !strcmp(key, "visibility") || !strcmp(key, "moon_phase") || !strcmp(key, "date") || !strcmp(key, "value"))
                {
                    printf("JSON Key: \"%s\" = ", key);
                    switch (json_typeof(element)) {

                        case JSON_STRING:
                            print_json_string(element, indent);
                            break;
                        case JSON_INTEGER:
                            print_json_integer(element, indent);
                            break;
                        case JSON_REAL:
                            print_json_real(element, indent);
                            break;
                        case JSON_TRUE:
                            print_json_true(element, indent);
                            break;
                        case JSON_FALSE:
                            print_json_false(element, indent);
                            break;
                        case JSON_NULL:
                            print_json_null(element, indent);
                            break;
                        default:
                            fprintf(stderr, "unrecognized JSON type %d\n", json_typeof(element));
                    }
                }
    }
}

void print_json_indent(int indent) {
    int i;
    for (i = 0; i < indent; i++) {
        putchar(' ');
    }
}

const char *json_plural(size_t count) { return count == 1 ? "" : "s"; }

void print_json_object(char *parent_key, json_t *element, int indent) {
    size_t size;
    const char *key;
    json_t *value;

    size = json_object_size(element);

    json_object_foreach(element, key, value) {
        print_json_aux(parent_key, key, value, indent + 2);
    }
}

void print_json_array(char *parent_key, char *key, json_t *element, int indent) {
    size_t i;
    size_t size = json_array_size(element);

    for (i = 0; i < size; i++) {
        print_json_aux(parent_key, key, json_array_get(element, i), indent + 2);
    }
}

void print_json_string(json_t *element, int indent) {
    print_json_indent(indent);
    printf("string: \"%s\"\n", json_string_value(element));
}

void print_json_integer(json_t *element, int indent) {
    print_json_indent(indent);
    printf("integer: \"%" JSON_INTEGER_FORMAT "\"\n", json_integer_value(element));
}

void print_json_real(json_t *element, int indent) {
    print_json_indent(indent);
    printf("real: %f\n", json_real_value(element));
}

void print_json_true(json_t *element, int indent) {
    (void)element;
    print_json_indent(indent);
    printf("true\n");
}

void print_json_false(json_t *element, int indent) {
    (void)element;
    print_json_indent(indent);
    printf("false\n");
}

void print_json_null(json_t *element, int indent) {
    (void)element;
    print_json_indent(indent);
    printf("null\n");
}

/*
 * Parse text into a JSON object. If text is valid JSON, returns a
 * json_t structure, otherwise prints and error and returns null.
 */
json_t *load_json_(const char *text) {
    json_t *root;
    json_error_t error;

    root = json_loads(text, 0, &error);

    if (root) {
        return root;
    } else {
        fprintf(stderr, "json error on line %d: %s\n", error.line, error.text);
        return (json_t *)0;
    }
}

/*
 * Print a prompt and return (by reference) a null-terminated line of
 * text.  Returns NULL on eof or some error.
 */
char *read_line(char *line, int max_chars) {
    printf("Type some JSON > ");
    fflush(stdout);
    return fgets(line, max_chars, stdin);
}


struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

  return realsize;
}

static size_t WriteFileCallback(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

void prt_json(json_t *root)
{
    json_t *value;
    json_t *nearest_area = json_object_get(root, "nearest_area");
    nearest_area = json_array_get(nearest_area, 0);

    value = json_object_get(nearest_area, "region");
    value = json_array_get(value, 0);
    value = json_object_get(value, "value");
    char* txt = json_string_value(value);
    printf("%s\n", txt);

    value = json_object_get(nearest_area, "country");
    value = json_array_get(value, 0);
    value = json_object_get(value, "value");
    txt = json_string_value(value);
    printf("%s\n", txt);

    json_t *current_condition = json_object_get(root, "current_condition");
    current_condition = json_array_get(current_condition, 0);

    json_t *lang_ru = json_object_get(current_condition, "weatherDesc");
    lang_ru = json_array_get(lang_ru, 0);
    value = json_object_get(lang_ru, "value");
    txt = json_string_value(value);
    printf("Погода - %s\n", txt);

    value = json_object_get(current_condition, "temp_C");
    txt = json_string_value(value);
    printf("Температура = %s C\n", txt);

    value = json_object_get(current_condition, "FeelsLikeC");
    txt = json_string_value(value);
    printf("Чувствуется как = %s C\n", txt);

    value = json_object_get(current_condition, "pressure");
    txt = json_string_value(value);
    printf("Давление = %s\n", txt);

    value = json_object_get(current_condition, "visibility");
    txt = json_string_value(value);
    printf("Видимость = %s\n", txt);

    value = json_object_get(current_condition, "localObsDateTime");
    txt = json_string_value(value);
    printf("%s\n", txt);

    json_t *weather = json_object_get(root, "weather");
    weather = json_array_get(weather, 0);
    json_t *astronomy = json_object_get(weather, "astronomy");
    astronomy = json_array_get(astronomy, 0);
    value = json_object_get(astronomy, "moon_illumination");
    txt = json_string_value(value);
    printf("Moon_illumination - %s\n", txt);

    value = json_object_get(astronomy, "moon_phase");
    txt = json_string_value(value);
    printf("Moon_phase - %s\n", txt);

    value = json_object_get(astronomy, "moonrise");
    txt = json_string_value(value);
    printf("Moonrise - %s\n", txt);

    value = json_object_get(astronomy, "moonset");
    txt = json_string_value(value);
    printf("Moonset - %s\n", txt);

    value = json_object_get(astronomy, "sunrise");
    txt = json_string_value(value);
    printf("Sunrise - %s\n", txt);

    value = json_object_get(astronomy, "sunset");
    txt = json_string_value(value);
    printf("Sunset - %s\n", txt);
}

char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}


int main(int argc, char **argv) {
    if(argc == 2)
    {
        CURL *curl;
        CURLcode res;
        curl_global_init(CURL_GLOBAL_ALL);
        curl = curl_easy_init();

        struct MemoryStruct readBuffer;

        readBuffer.memory = malloc(1);
        readBuffer.size = 0;

        long response_code;
        if(curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, concat(concat("https://wttr.in/", argv[1]), "?format=j1"));
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&readBuffer);
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
            res = curl_easy_perform(curl);  /* Check for errors */
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);

            if(res != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            }
            else {
                /*
                 * Now, our chunk.memory points to a memory block that is chunk.size
                 * bytes big and contains the remote file.
                 *
                 * Do something nice with it!
                 */

                printf("%lu bytes retrieved\n", (unsigned long)readBuffer.size);
            }
            curl_easy_cleanup(curl);
        }

        printf("Response code %ld\n", response_code);

        json_t *root = load_json_(readBuffer.memory);

        if (root) {
                prt_json(root);
                //print_json(root);
                json_decref(root);
        }

        free(readBuffer.memory);

        curl_global_cleanup();
    }
    return 0;
}
