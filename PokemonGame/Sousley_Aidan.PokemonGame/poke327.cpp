#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <sys/select.h>
#include <unistd.h>
#include <ncurses.h>
#include <string>
#include <vector>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "heap.h"

std::string int_or_na(int val) {
    return val == INT_MAX ? "N/A" : std::to_string(val);
}

#define malloc(size) ({          \
  void *_tmp;                    \
  assert((_tmp = (queue_node_t *) malloc(size))); \
  _tmp;                          \
})

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef int16_t pair_t[num_dims];

#define MAP_X              80
#define MAP_Y              21
#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95
#define WORLD_SIZE         401

#define MOUNTAIN_SYMBOL       '%'
#define BOULDER_SYMBOL        '0'
#define TREE_SYMBOL           '4'
#define FOREST_SYMBOL         '^'
#define GATE_SYMBOL           '#'
#define PATH_SYMBOL           '#'
#define POKEMART_SYMBOL       'M'
#define POKEMON_CENTER_SYMBOL 'C'
#define TALL_GRASS_SYMBOL     ':'
#define SHORT_GRASS_SYMBOL    '.'
#define WATER_SYMBOL          '~'
#define ERROR_SYMBOL          '&'

#define HIKER_SYMBOL          'h'
#define RIVAL_SYMBOL          'r'
#define PACER_SYMBOL          'p'
#define WANDERER_SYMBOL       'w'
#define SENTRY_SYMBOL         's'
#define EXPLORER_SYMBOL       'e'

#define DIJKSTRA_PATH_MAX (INT_MAX / 2)

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

#define BATTLE_START_ROW 10

uint32_t world_time = 0;
int num_trainers = 10;

typedef enum __attribute__ ((__packed__)) terrain_type {
  ter_boulder,
  ter_tree,
  ter_path,
  ter_mart,
  ter_center,
  ter_grass,
  ter_clearing,
  ter_mountain,
  ter_forest,
  ter_water,
  ter_gate,
  num_terrain_types,
  ter_debug
} terrain_type_t;

typedef enum __attribute__ ((__packed__)) character_type {
  char_hiker,
  char_rival,
  char_pacer,
  char_wanderer,
  char_sentry,
  char_explorer,
} character_type_t;

class Move  {
public:
    int id;
    char identifier[50];
    int generation_id;
    int type_id;
    int power;
    int pp;
    int accuracy;
    int priority;
    int target_id;
    int damage_class_id;
    int effect_id;
    int effect_chance;
    int contest_type_id;
    int contest_effect_id;
    int super_contest_effect_id;

    Move(int id, const char *identifier, int generation_id, int type_id, int power, int pp, int accuracy, int priority, int target_id, int damage_class_id,
         int effect_id, int effect_chance, int contest_type_id, int contest_effect_id, int super_contest_effect_id) {
        this->id = id;
        strncpy(this->identifier, identifier, sizeof(this->identifier) - 1);
        this->identifier[sizeof(this->identifier) - 1] = '\0';
        this->generation_id = generation_id;
        this->type_id = type_id;
        this->power = power;
        this->pp = pp;
        this->accuracy = accuracy;
        this->priority = priority;
        this->target_id = target_id;
        this->damage_class_id = damage_class_id;
        this->effect_id = effect_id;
        this->effect_chance = effect_chance;
        this->contest_type_id = contest_type_id;
        this->contest_effect_id = contest_effect_id;
        this->super_contest_effect_id = super_contest_effect_id;
    }

    Move() {};

    void parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        strncpy(identifier, token.c_str(), sizeof(identifier) - 1);
        identifier[sizeof(identifier) - 1] = '\0';

        std::getline(ss, token, ',');
        generation_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        type_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        power = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        pp = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        accuracy = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        priority = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        target_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        damage_class_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        effect_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        effect_chance = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        contest_type_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        contest_effect_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        super_contest_effect_id = token.empty() ? INT_MAX : std::stoi(token);
    }
};

class Pokemon  {
public:
    int id;
    char identifier[50];
    int height;
    int weight;
    int base_experience;
    int order;
    int is_default;
    int level;
    std::vector<Move> moves;
    int hp;
    int attack;
    int defense;
    int speed;
    int special_attack;
    int special_defense;
    int gender; 
    bool is_shiny;
    int current_hp;

    Pokemon(int id, const char *identifier, int height, int weight, int base_experience, int order, int is_default) {
        this->id = id;
        strncpy(this->identifier, identifier, sizeof(this->identifier) - 1);
        this->identifier[sizeof(this->identifier) - 1] = '\0';
        this->height = height;
        this->weight = weight;
        this->base_experience = base_experience;
        this->order = order;
        this->is_default = is_default;
        this->level = 0;
    }

    Pokemon() {};

    void parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        strncpy(identifier, token.c_str(), sizeof(identifier) - 1);
        identifier[sizeof(identifier) - 1] = '\0';

        std::getline(ss, token, ',');
        height = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        weight = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        base_experience = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        order = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        is_default = token.empty() ? INT_MAX : std::stoi(token);
    }
};

class PokemonMove  {
  public:
    int pokemon_id;
    int version_group_id;
    int move_id;
    int pokemon_move_method_id;
    int level;
    int order;

    PokemonMove(int pokemon_id, int version_group_id, int move_id, int pokemon_move_method_id, int level, int order) {
        this->pokemon_id = pokemon_id;
        this->version_group_id = version_group_id;
        this->move_id = move_id;
        this->pokemon_move_method_id = pokemon_move_method_id;
        this->level = level;
        this->order = order;
    }

    PokemonMove() {};

    void parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        pokemon_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        version_group_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        move_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        pokemon_move_method_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        level = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        order = token.empty() ? INT_MAX : std::stoi(token);
    }
};

class PokemonSpecies  {
  public:
    int id;
    char identifier[50];
    int generation_id;
    int evolves_from_species_id;
    int evolution_chain_id;
    int color_id;
    int shape_id;
    int habitat_id;
    int gender_rate;
    int capture_rate;
    int base_happiness;
    int is_baby;
    int hatch_counter;
    int has_gender_differences;
    int growth_rate_id;
    int forms_switchable;
    int is_legendary;
    int is_mythical;
    int order;
    int conquest_order;

    PokemonSpecies(int id, const char *identifier, int generation_id, int evolves_from_species_id, int evolution_chain_id, int color_id,
                   int shape_id, int habitat_id, int gender_rate, int capture_rate, int base_happiness, int is_baby, int hatch_counter,
                   int has_gender_differences, int growth_rate_id, int forms_switchable, int is_legendary, int is_mythical, int order, int conquest_order) {
        this->id = id;
        strncpy(this->identifier, identifier, sizeof(this->identifier) - 1);
        this->identifier[sizeof(this->identifier) - 1] = '\0';
        this->generation_id = generation_id;
        this->evolves_from_species_id = evolves_from_species_id;
        this->evolution_chain_id = evolution_chain_id;
        this->color_id = color_id;
        this->shape_id = shape_id;
        this->habitat_id = habitat_id;
        this->gender_rate = gender_rate;
        this->capture_rate = capture_rate;
        this->base_happiness = base_happiness;
        this->is_baby = is_baby;
        this->hatch_counter = hatch_counter;
        this->has_gender_differences = has_gender_differences;
        this->growth_rate_id = growth_rate_id;
        this->forms_switchable = forms_switchable;
        this->is_legendary = is_legendary;
        this->is_mythical = is_mythical;
        this->order = order;
        this->conquest_order = conquest_order;
    }

    PokemonSpecies() {};

    void parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        strncpy(identifier, token.c_str(), sizeof(identifier) - 1);
        identifier[sizeof(identifier) - 1] = '\0';

        std::getline(ss, token, ',');
        generation_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        evolves_from_species_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        evolution_chain_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        color_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        shape_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        habitat_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        gender_rate = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        capture_rate = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        base_happiness = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        is_baby = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        hatch_counter = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        has_gender_differences = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        growth_rate_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        forms_switchable = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        is_legendary = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        is_mythical = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        order = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        conquest_order = token.empty() ? INT_MAX : std::stoi(token);
    }
};

class Experience  {
  public:
    int growth_rate_id;
    int level;
    int experience;

    Experience(int growth_rate_id, int level, int experience){
        this->growth_rate_id = growth_rate_id;
        this->level = level;
        this->experience = experience;
    }

    Experience(){};

    void parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        growth_rate_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        level = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        experience = token.empty() ? INT_MAX : std::stoi(token);
    }
};

class TypeName  {
  public:
    int type_id;
    int local_language_id;
    char name[50];

    TypeName(int type_id, int local_language_id, const char *name){
        this->type_id = type_id;
        this->local_language_id = local_language_id;
        strncpy(this->name, name, sizeof(this->name) - 1);
        this->name[sizeof(this->name) - 1] = '\0';
    }

    TypeName(){};

    void parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        type_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        local_language_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        strncpy(name, token.c_str(), sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0';
    }
};

class PokemonStat  {
  public:
    int pokemon_id;
    int stat_id;
    int base_stat;
    int effort;

    PokemonStat(int pokemon_id, int stat_id, int base_stat, int effort){
        this->pokemon_id = pokemon_id;
        this->stat_id = stat_id;
        this->base_stat = base_stat;
        this->effort = effort;
    }

    PokemonStat(){};

    void parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        pokemon_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        stat_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        base_stat = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        effort = token.empty() ? INT_MAX : std::stoi(token);
    }
};

class Stats  {
  public:
    int id;
    char identifier[50];
    int is_battle_only;
    int game_index;
    int damage_class_id;

    Stats(int id, int damage_class_id, const char *identifier, int is_battle_only, int game_index){
        this->id = id;
        this->damage_class_id = damage_class_id;
        strncpy(this->identifier, identifier, sizeof(this->identifier) - 1);
        this->identifier[sizeof(this->identifier) - 1] = '\0';
        this->is_battle_only = is_battle_only;
        this->game_index = game_index;
    }

    Stats(){};
    
    void parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        damage_class_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        strncpy(identifier, token.c_str(), sizeof(identifier) - 1);
        identifier[sizeof(identifier) - 1] = '\0';

        std::getline(ss, token, ',');
        is_battle_only = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        game_index = token.empty() ? INT_MAX : std::stoi(token);
    }
};

class PokemonTypes {
  public:
    int pokemon_id;
    int type_id;
    int slot;

    PokemonTypes(int pokemon_id, int type_id, int slot){
        this->pokemon_id = pokemon_id;
        this->type_id = type_id;
        this->slot = slot;
    }

    PokemonTypes(){};

    void parseLine(const std::string& line) {
        std::stringstream ss(line);
        std::string token;

        std::getline(ss, token, ',');
        pokemon_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        type_id = token.empty() ? INT_MAX : std::stoi(token);

        std::getline(ss, token, ',');
        slot = token.empty() ? INT_MAX : std::stoi(token);
    }
};

class Character {
public:
  character_type_t type;
  int x;
  int y;
  int next_turn;
  int direction;
  int defeated;
  std::vector<Pokemon> pokemon;
};

class Bag {
  public:
    int num_pokeballs;
    int num_potions;
    int num_revives;
};

class PC : public Character {
  public:
    Bag bag;
};

class NPC : public Character {
};

class Map {
public:
  terrain_type_t map[MAP_Y][MAP_X];
  uint8_t height[MAP_Y][MAP_X];
  int8_t n, s, e, w;
  Character *character_map[MAP_Y][MAP_X];
  heap_t turn_heap;
};


class Trainer_Battle{
public:
  Character *npc;
  int npc_pokemon_index;
  bool is_battle_over;
  Pokemon *pc_pokemon;

  Trainer_Battle(Character *npc, int npc_pokemon_index, Pokemon *pc_pokemon) {
    this->npc = npc;
    this->npc_pokemon_index = npc_pokemon_index;
    this->is_battle_over = false;
    this->pc_pokemon = pc_pokemon;
  }
};

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

class World {
public:
  Map *world[WORLD_SIZE][WORLD_SIZE];
  pair_t cur_idx;
  Map *cur_map;
  /* Place distance maps in world, not map, since *
   * we only need one pair at any given time.     */
  int hiker_dist[MAP_Y][MAP_X];
  int rival_dist[MAP_Y][MAP_X];
  PC pc;
};


std::unordered_map<int, Pokemon> pokemon_map;
std::unordered_map<int, int> pokemon_type_map;
std::vector<Pokemon> pokemon_data;
std::unordered_map<int, Move> move_map;
std::unordered_map<int, std::vector<PokemonMove>> moves_by_pokemon;
std::unordered_map<int, PokemonStat> pokemon_stats_map;

/* Even unallocated, a WORLD_SIZE x WORLD_SIZE array of pointers is a very *
 * large thing to put on the stack.  To avoid that, world is a global.     */
World world;

/* Just to make the following table fit in 80 columns */
#define IM DIJKSTRA_PATH_MAX
int32_t move_cost[4][num_terrain_types] = {
//  boulder,tree,path,mart,center,grass,clearing,mountain,forest,water,gate
  { IM, IM, 10, 10, 10, 20, 10, IM, IM, IM, 10 },
  { IM, IM, 10, 50, 50, 15, 10, 15, 15, IM, IM },
  { IM, IM, 10, 50, 50, 20, 10, IM, IM, IM, IM },
  { IM, IM, IM, IM, IM, IM, IM, IM, IM,  7, IM },
};
#undef IM

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t edge_penalty(int8_t x, int8_t y)
{
  return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static void dijkstra_path(Map *m, pair_t from, pair_t to)
{
  static path_t path[MAP_Y][MAP_X], *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = DIJKSTRA_PATH_MAX;
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = (path_t *) heap_remove_min(&h))) {
    p->hn = NULL;

    if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
      for (x = to[dim_x], y = to[dim_y];
           (x != from[dim_x]) || (y != from[dim_y]);
           p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
        /* Don't overwrite the gate */
        if (x != to[dim_x] || y != to[dim_y]) {
          mapxy(x, y) = ter_path;
          heightxy(x, y) = 0;
        }
      }
      heap_delete(&h);
      return;
    }

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                           [p->pos[dim_x]    ].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] - 1].hn);
    }
    if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
        (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
      path[p->pos[dim_y]][p->pos[dim_x] + 1].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                           [p->pos[dim_x] + 1].hn);
    }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
        (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
         ((p->cost + heightpair(p->pos)) *
          edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
        ((p->cost + heightpair(p->pos)) *
         edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
      path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
      heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                           [p->pos[dim_x]    ].hn);
    }
  }
}

static int build_paths(Map *m)
{
  pair_t from, to;

  /*  printf("%d %d %d %d\n", m->n, m->s, m->e, m->w);*/

  if (m->e != -1 && m->w != -1) {
    from[dim_x] = 1;
    to[dim_x] = MAP_X - 2;
    from[dim_y] = m->w;
    to[dim_y] = m->e;

    dijkstra_path(m, from, to);
  }

  if (m->n != -1 && m->s != -1) {
    from[dim_y] = 1;
    to[dim_y] = MAP_Y - 2;
    from[dim_x] = m->n;
    to[dim_x] = m->s;

    dijkstra_path(m, from, to);
  }

  if (m->e == -1) {
    if (m->s == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->w == -1) {
    if (m->s == -1) {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->n == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->s;
      to[dim_y] = MAP_Y - 2;
    }

    dijkstra_path(m, from, to);
  }

  if (m->s == -1) {
    if (m->e == -1) {
      from[dim_x] = 1;
      from[dim_y] = m->w;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    } else {
      from[dim_x] = MAP_X - 2;
      from[dim_y] = m->e;
      to[dim_x] = m->n;
      to[dim_y] = 1;
    }

    dijkstra_path(m, from, to);
  }

  return 0;
}

static int gaussian[5][5] = {
  {  1,  4,  7,  4,  1 },
  {  4, 16, 26, 16,  4 },
  {  7, 26, 41, 26,  7 },
  {  4, 16, 26, 16,  4 },
  {  1,  4,  7,  4,  1 }
};

static int smooth_height(Map *m)
{
  int32_t i, x, y;
  int32_t s, t, p, q;
  queue_node_t *head, *tail, *tmp;
  /*  FILE *out;*/
  uint8_t height[MAP_Y][MAP_X];

  memset(&height, 0, sizeof (height));

  /* Seed with some values */
  for (i = 1; i < 255; i += 20) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (height[y][x]);
    height[y][x] = i;
    if (i == 1) {
      head = tail = (queue_node_t *) malloc(sizeof (*tail));
    } else {
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);
  */
  
  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = height[y][x];

    if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
      height[y - 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y - 1;
    }
    if (x - 1 >= 0 && !height[y][x - 1]) {
      height[y][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y;
    }
    if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
      height[y + 1][x - 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x - 1;
      tail->y = y + 1;
    }
    if (y - 1 >= 0 && !height[y - 1][x]) {
      height[y - 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y - 1;
    }
    if (y + 1 < MAP_Y && !height[y + 1][x]) {
      height[y + 1][x] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x;
      tail->y = y + 1;
    }
    if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
      height[y - 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y - 1;
    }
    if (x + 1 < MAP_X && !height[y][x + 1]) {
      height[y][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y;
    }
    if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
      height[y + 1][x + 1] = i;
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
      tail->next = NULL;
      tail->x = x + 1;
      tail->y = y + 1;
    }

    tmp = head;
    head = head->next;
    free(tmp);
  }

  /* And smooth it a bit with a gaussian convolution */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }
  /* Let's do it again, until it's smooth like Kenny G. */
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      for (s = t = p = 0; p < 5; p++) {
        for (q = 0; q < 5; q++) {
          if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
              x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
            s += gaussian[p][q];
            t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
          }
        }
      }
      m->height[y][x] = t / s;
    }
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&height, sizeof (height), 1, out);
  fclose(out);

  out = fopen("smoothed.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->height, sizeof (m->height), 1, out);
  fclose(out);
  */

  return 0;
}

static void find_building_location(Map *m, pair_t p)
{
  do {
    p[dim_x] = rand() % (MAP_X - 3) + 1;
    p[dim_y] = rand() % (MAP_Y - 3) + 1;

    if ((((mapxy(p[dim_x] - 1, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x] + 2, p[dim_y]    ) == ter_path)     &&
          (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] - 1) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path))    ||
         ((mapxy(p[dim_x]    , p[dim_y] + 2) == ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path)))   &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_center)   &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_center)   &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
        (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_path)     &&
          (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_path)     &&
          (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path)))) {
          break;
    }
  } while (1);
}

static int place_pokemart(Map *m)
{
  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_mart;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_mart;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

  return 0;
}

static int place_center(Map *m)
{  pair_t p;

  find_building_location(m, p);

  mapxy(p[dim_x]    , p[dim_y]    ) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_center;
  mapxy(p[dim_x]    , p[dim_y] + 1) = ter_center;
  mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

  return 0;
}

/* Chooses tree or boulder for border cell.  Choice is biased by dominance *
 * of neighboring cells.                                                   */
static terrain_type_t border_type(Map *m, int32_t x, int32_t y)
{
  int32_t p, q;
  int32_t r, t;
  int32_t miny, minx, maxy, maxx;
  
  r = t = 0;
  
  miny = y - 1 >= 0 ? y - 1 : 0;
  maxy = y + 1 <= MAP_Y ? y + 1: MAP_Y;
  minx = x - 1 >= 0 ? x - 1 : 0;
  maxx = x + 1 <= MAP_X ? x + 1: MAP_X;

  for (q = miny; q < maxy; q++) {
    for (p = minx; p < maxx; p++) {
      if (q != y || p != x) {
        if (m->map[q][p] == ter_mountain ||
            m->map[q][p] == ter_boulder) {
          r++;
        } else if (m->map[q][p] == ter_forest ||
                   m->map[q][p] == ter_tree) {
          t++;
        }
      }
    }
  }
  
  if (t == r) {
    return rand() & 1 ? ter_boulder : ter_tree;
  } else if (t > r) {
    if (rand() % 10) {
      return ter_tree;
    } else {
      return ter_boulder;
    }
  } else {
    if (rand() % 10) {
      return ter_boulder;
    } else {
      return ter_tree;
    }
  }
}

static int Maperrain(Map *m, int8_t n, int8_t s, int8_t e, int8_t w)
{
  int32_t i, x, y;
  queue_node_t *head, *tail, *tmp;
  //  FILE *out;
  int num_grass, num_clearing, num_mountain, num_forest, num_water, num_total;
  terrain_type_t type;
  int added_current = 0;
  
  num_grass = rand() % 4 + 2;
  num_clearing = rand() % 4 + 2;
  num_mountain = rand() % 2 + 1;
  num_forest = rand() % 2 + 1;
  num_water = rand() % 2 + 1;
  num_total = num_grass + num_clearing + num_mountain + num_forest + num_water;

  memset(&m->map, 0, sizeof (m->map));

  /* Seed with some values */
  for (i = 0; i < num_total; i++) {
    do {
      x = rand() % MAP_X;
      y = rand() % MAP_Y;
    } while (m->map[y][x]);
    if (i == 0) {
      type = ter_grass;
    } else if (i == num_grass) {
      type = ter_clearing;
    } else if (i == num_grass + num_clearing) {
      type = ter_mountain;
    } else if (i == num_grass + num_clearing + num_mountain) {
      type = ter_forest;
    } else if (i == num_grass + num_clearing + num_mountain + num_forest) {
      type = ter_water;
    }
    m->map[y][x] = type;
    if (i == 0) {
      head = tail = (queue_node_t *) malloc(sizeof (*tail));
    } else {
      tail->next = (queue_node_t *) malloc(sizeof (*tail));
      tail = tail->next;
    }
    tail->next = NULL;
    tail->x = x;
    tail->y = y;
  }

  /*
  out = fopen("seeded.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */

  /* Diffuse the vaules to fill the space */
  while (head) {
    x = head->x;
    y = head->y;
    i = m->map[y][x];
    
    if (x - 1 >= 0 && !m->map[y][x - 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x - 1] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x - 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y - 1 >= 0 && !m->map[y - 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y - 1][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y - 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
      if ((rand() % 100) < 20) {
        m->map[y + 1][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y + 1;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    if (x + 1 < MAP_X && !m->map[y][x + 1]) {
      if ((rand() % 100) < 80) {
        m->map[y][x + 1] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x + 1;
        tail->y = y;
      } else if (!added_current) {
        added_current = 1;
        m->map[y][x] = (terrain_type_t) i;
        tail->next = (queue_node_t *) malloc(sizeof (*tail));
        tail = tail->next;
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
      }
    }

    added_current = 0;
    tmp = head;
    head = head->next;
    free(tmp);
  }

  /*
  out = fopen("diffused.pgm", "w");
  fprintf(out, "P5\n%u %u\n255\n", MAP_X, MAP_Y);
  fwrite(&m->map, sizeof (m->map), 1, out);
  fclose(out);
  */
  
  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (y == 0 || y == MAP_Y - 1 ||
          x == 0 || x == MAP_X - 1) {
        mapxy(x, y) = border_type(m, x, y);
      }
    }
  }

  m->n = n;
  m->s = s;
  m->e = e;
  m->w = w;

  if (n != -1) {
    mapxy(n,         0        ) = ter_gate;
    mapxy(n,         1        ) = ter_gate;
  }
  if (s != -1) {
    mapxy(s,         MAP_Y - 1) = ter_gate;
    mapxy(s,         MAP_Y - 2) = ter_gate;
  }
  if (w != -1) {
    mapxy(0,         w        ) = ter_gate;
    mapxy(1,         w        ) = ter_gate;
  }
  if (e != -1) {
    mapxy(MAP_X - 1, e        ) = ter_gate;
    mapxy(MAP_X - 2, e        ) = ter_gate;
  }

  return 0;
}

static int place_boulders(Map *m)
{
  int i;
  int x, y;

  for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_forest &&
        m->map[y][x] != ter_path   &&
        m->map[y][x] != ter_gate) {
      m->map[y][x] = ter_boulder;
    }
  }

  return 0;
}

static int place_trees(Map *m)
{
  int i;
  int x, y;
  
  for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
    y = rand() % (MAP_Y - 2) + 1;
    x = rand() % (MAP_X - 2) + 1;
    if (m->map[y][x] != ter_mountain &&
        m->map[y][x] != ter_path     &&
        m->map[y][x] != ter_water    &&
        m->map[y][x] != ter_gate) {
      m->map[y][x] = ter_tree;
    }
  }

  return 0;
}

int compare_turns(const void *a, const void *b)
{
  Character *c1 = (Character *) a;
  Character *c2 = (Character *) b;

  if (c1->next_turn < c2->next_turn) {
    return -1;
  } else if (c1->next_turn > c2->next_turn) {
    return 1;
  } else {
    return 0;
  }
}

// New map expects cur_idx to refer to the index to be generated.  If that
// map has already been generated then the only thing this does is set
// cur_map.
static int new_map()
{
  int d, p;
  int e, w, n, s;

  if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]]) {
    world.cur_map = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]];
    return 0;
  }
  world.cur_map                                             =
    world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x]] =
    new Map();

  heap_init(&world.cur_map->turn_heap, compare_turns, NULL);

  smooth_height(world.cur_map);
  
  if (!world.cur_idx[dim_y]) {
    n = -1;
  } else if (world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]) {
    n = world.world[world.cur_idx[dim_y] - 1][world.cur_idx[dim_x]]->s;
  } else {
    n = 1 + rand() % (MAP_X - 2);
  }
  if (world.cur_idx[dim_y] == WORLD_SIZE - 1) {
    s = -1;
  } else if (world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]) {
    s = world.world[world.cur_idx[dim_y] + 1][world.cur_idx[dim_x]]->n;
  } else  {
    s = 1 + rand() % (MAP_X - 2);
  }
  if (!world.cur_idx[dim_x]) {
    w = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]) {
    w = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] - 1]->e;
  } else {
    w = 1 + rand() % (MAP_Y - 2);
  }
  if (world.cur_idx[dim_x] == WORLD_SIZE - 1) {
    e = -1;
  } else if (world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]) {
    e = world.world[world.cur_idx[dim_y]][world.cur_idx[dim_x] + 1]->w;
  } else {
    e = 1 + rand() % (MAP_Y - 2);
  }
  
  Maperrain(world.cur_map, n, s, e, w);
     
  place_boulders(world.cur_map);
  place_trees(world.cur_map);
  build_paths(world.cur_map);
  d = (abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)) +
       abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)));
  p = d > 200 ? 5 : (50 - ((45 * d) / 200));
  //  printf("d=%d, p=%d\n", d, p);
  if ((rand() % 100) < p || !d) {
    place_pokemart(world.cur_map);
  }
  if ((rand() % 100) < p || !d) {
    place_center(world.cur_map);
  }

  return 1;
}

static void print_map()
{
  int x, y;
  int default_reached = 0;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      char to_draw = ERROR_SYMBOL;
      if (world.pc.y == y &&
          world.pc.x == x) {
        attron(COLOR_PAIR(5));
        to_draw = '@';
        mvaddch(y+1, x, to_draw);
        attroff(COLOR_PAIR(5));
      }
      else if(world.cur_map->character_map[y][x]){
        switch (world.cur_map->character_map[y][x]->type) {
          case char_hiker:
            to_draw = HIKER_SYMBOL;
            attron(COLOR_PAIR(5));
            mvaddch(y+1, x, to_draw);
            attroff(COLOR_PAIR(5));
            break;
          case char_rival:
            to_draw = RIVAL_SYMBOL;
            attron(COLOR_PAIR(5));
            mvaddch(y+1, x, to_draw);
            attroff(COLOR_PAIR(5));
            break;
          case char_pacer:
            to_draw = PACER_SYMBOL;
            attron(COLOR_PAIR(5));
            mvaddch(y+1, x, to_draw);
            attroff(COLOR_PAIR(5));
            break;
          case char_wanderer:
            to_draw = WANDERER_SYMBOL;
            attron(COLOR_PAIR(5));
            mvaddch(y+1, x, to_draw);
            attroff(COLOR_PAIR(5));
            break;
          case char_sentry:
            to_draw = SENTRY_SYMBOL;
            attron(COLOR_PAIR(5));
            mvaddch(y+1, x, to_draw);
            attroff(COLOR_PAIR(5));
            break;
          case char_explorer:
            to_draw = EXPLORER_SYMBOL;
            attron(COLOR_PAIR(5));
            mvaddch(y+1, x, to_draw);
            attroff(COLOR_PAIR(5));
            break;
        }
      }
      else {
        switch (world.cur_map->map[y][x]) {
        case ter_boulder:
          to_draw = BOULDER_SYMBOL;
          attron(COLOR_PAIR(4));
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(4));
          break;
        case ter_mountain:
          to_draw = MOUNTAIN_SYMBOL;
          attron(COLOR_PAIR(4));
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(4));
          break;
        case ter_tree:
          to_draw = TREE_SYMBOL;
          attron(COLOR_PAIR(6));
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(6));
          break;
        case ter_forest:
          to_draw = FOREST_SYMBOL;
          attron(COLOR_PAIR(6));
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(6));
          break;
        case ter_path:
          to_draw = PATH_SYMBOL;
          attron(COLOR_PAIR(1));
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(1));
          break;
        case ter_gate:
          to_draw = GATE_SYMBOL;
          attron(COLOR_PAIR(1));
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(1));
          break;
        case ter_mart:
          to_draw = POKEMART_SYMBOL;
          attron(COLOR_PAIR(3));
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(3));
          break;
        case ter_center:
          to_draw = POKEMON_CENTER_SYMBOL;
          attron(COLOR_PAIR(3));
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(3));
          break;
        case ter_grass:
          to_draw = TALL_GRASS_SYMBOL;
            attron(COLOR_PAIR(2));
            mvaddch(y+1, x, to_draw);
            attroff(COLOR_PAIR(2));
            break;
          mvaddch(y+1, x, to_draw);
          break;
        case ter_clearing:
          attron(COLOR_PAIR(2));
          to_draw = SHORT_GRASS_SYMBOL;
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(2));
          break;
        case ter_water:
          attron(COLOR_PAIR(7));
          to_draw = WATER_SYMBOL;
          mvaddch(y+1, x, to_draw);
          attroff(COLOR_PAIR(7));
          break;
        default:
          to_draw = ERROR_SYMBOL;
          default_reached = 1;
          break;
        }
      }
    }
  }

  if (default_reached) {
    mvprintw(0, 0, "Default reached in %s\n", __FUNCTION__);
  }

  refresh();
}

// The world is global because of its size, so init_world is parameterless
void init_world()
{
  world.cur_idx[dim_x] = world.cur_idx[dim_y] = WORLD_SIZE / 2;
  new_map();
}

void delete_world()
{
  int x, y;

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      if (world.world[y][x]) {
        free(world.world[y][x]);
        world.world[y][x] = NULL;
      }
    }
  }
}

#define ter_cost(x, y, c) move_cost[c][m->map[y][x]]

static int32_t hiker_cmp(const void *key, const void *with) {
  return (world.hiker_dist[((path_t *) key)->pos[dim_y]]
                          [((path_t *) key)->pos[dim_x]] -
          world.hiker_dist[((path_t *) with)->pos[dim_y]]
                          [((path_t *) with)->pos[dim_x]]);
}

static int32_t rival_cmp(const void *key, const void *with) {
  return (world.rival_dist[((path_t *) key)->pos[dim_y]]
                          [((path_t *) key)->pos[dim_x]] -
          world.rival_dist[((path_t *) with)->pos[dim_y]]
                          [((path_t *) with)->pos[dim_x]]);
}

void pathfind(Map *m)
{
  heap_t h;
  uint32_t x, y;
  static path_t p[MAP_Y][MAP_X], *c;
  static uint32_t initialized = 0;

  if (!initialized) {
    initialized = 1;
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        p[y][x].pos[dim_y] = y;
        p[y][x].pos[dim_x] = x;
      }
    }
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      world.hiker_dist[y][x] = world.rival_dist[y][x] = DIJKSTRA_PATH_MAX;
    }
  }
  world.hiker_dist[world.pc.y][world.pc.x] = 
    world.rival_dist[world.pc.y][world.pc.x] = 0;

  heap_init(&h, hiker_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (ter_cost(x, y, char_hiker) != DIJKSTRA_PATH_MAX) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      } else {
        p[y][x].hn = NULL;
      }
    }
  }

  while ((c = (path_t *) heap_remove_min(&h))) {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn) &&
        (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn) &&
        (world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn) &&
        (world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn) &&
        (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker))) {
      world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
        world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_hiker);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);

  heap_init(&h, rival_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      if (ter_cost(x, y, char_rival) != DIJKSTRA_PATH_MAX) {
        p[y][x].hn = heap_insert(&h, &p[y][x]);
      } else {
        p[y][x].hn = NULL;
      }
    }
  }

  while ((c = (path_t *) heap_remove_min(&h))) {
    c->hn = NULL;
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) &&
        (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn) &&
        (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) &&
        (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn) &&
        (world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn) &&
        (world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) &&
        (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn) &&
        (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn);
    }
    if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) &&
        (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
         world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
         ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival))) {
      world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] =
        world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
        ter_cost(c->pos[dim_x], c->pos[dim_y], char_rival);
      heap_decrease_key_no_replace(&h,
                                   p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
    }
  }
  heap_delete(&h);
}

void init_pc()
{
  int x, y;

  do {
    x = rand() % (MAP_X - 2) + 1;
    y = rand() % (MAP_Y - 2) + 1;
  } while (world.cur_map->map[y][x] != ter_path);

  world.pc.x = x;
  world.pc.y = y;
}

void print_hiker_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.hiker_dist[y][x] == DIJKSTRA_PATH_MAX) {
        printf("   ");
      } else {
        printf(" %02d", world.hiker_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

void print_rival_dist()
{
  int x, y;

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      if (world.rival_dist[y][x] == DIJKSTRA_PATH_MAX ||
          world.rival_dist[y][x] < 0) {
        printf("   ");
      } else {
        printf(" %02d", world.rival_dist[y][x] % 100);
      }
    }
    printf("\n");
  }
}

Pokemon spawn_pokemon(){

  int id = std::rand() % 1092;
  id++; 
  Pokemon p = pokemon_data[id];
  int level;

  int y_distance = abs(world.cur_idx[dim_y] - 200 );
  int x_distance = abs(world.cur_idx[dim_x] - 200 );
  int manhattan_distance = y_distance + x_distance;

  if (manhattan_distance <= 200){
    int max_level = manhattan_distance / 2;
    level = std::rand() % (max_level + 1) + 1;
  }  
  else{
    int min_level = (manhattan_distance - 200);
    level = std::rand() % 100;
    if (level < min_level){
      level = min_level;
    }
    if (level > 100){
      level = 100;
    }
  }
  p.level = level;

  auto &moves = moves_by_pokemon[id];
  PokemonMove min_level_move;
    for (const PokemonMove &move : moves){
      if (move.pokemon_move_method_id == 1){
        min_level_move = move;
        break;
      }
    }
  std::vector<PokemonMove> acceptable_moves;

  for (const PokemonMove &move : moves){
    if ((move.level < min_level_move.level) && (move.pokemon_move_method_id == 1)){
      min_level_move = move;
    }
    if ((move.level <= p.level) && (move.pokemon_move_method_id == 1)){

      acceptable_moves.push_back(move);
    }
  }

  if (acceptable_moves.empty()){
    acceptable_moves.push_back(min_level_move);
  }

  int num_acceptable_moves = std::min(2, (int) acceptable_moves.size());

  for (int i = 0; i < num_acceptable_moves; i++){
    int move_index = std::rand() % acceptable_moves.size();
    if (i == 1){
      if (acceptable_moves[move_index].move_id == p.moves[0].id){
        break;
      }
    }
    p.moves.push_back(move_map[acceptable_moves[move_index].move_id]);
    acceptable_moves.erase(acceptable_moves.begin() + move_index);
  }
  PokemonStat ps = pokemon_stats_map[id];

  int iv_hp = std::rand() % 16;
  int iv_attack = std::rand() % 16;
  int iv_defense = std::rand() % 16;
  int iv_speed = std::rand() % 16;
  int iv_spatk = std::rand() % 16;
  int iv_spdef = std::rand() % 16;

  p.hp = ((ps.base_stat + iv_hp) * 2 * level) / 100 + level + 10;
  p.current_hp = p.hp;
  p.attack  = ((ps.base_stat + iv_attack) * 2 * level) / 100 + 5;
  p.defense = ((ps.base_stat + iv_defense) * 2 * level) / 100 + 5;
  p.speed   = ((ps.base_stat + iv_speed) * 2 * level) / 100 + 5;
  p.special_attack  = ((ps.base_stat + iv_spatk) * 2 * level) / 100 + 5;
  p.special_defense = ((ps.base_stat + iv_spdef) * 2 * level) / 100 + 5;

  int gender = std::rand() % 1;
  if (gender == 0){
    p.gender = 1;
  } else {
    p.gender = 0;
  }

  int shiny_chance = std::rand() % 8192;
  if (shiny_chance == 0){
    p.is_shiny = true;
  } else {
    p.is_shiny = false;
  }

  return p;
}

void spawn_trainer(Character *c)
{
  int x,y;
  do {x = rand() % (MAP_X - 2) + 1;
      y = rand() % (MAP_Y - 2) + 1;}
   while (((world.cur_map->map[y][x] != ter_path) &&
           (world.cur_map->map[y][x] != ter_clearing) &&
           (world.cur_map->map[y][x] != ter_grass) &&
           (world.cur_map->map[y][x] != ter_mart) &&
           (world.cur_map->map[y][x] != ter_center)) ||
          (world.cur_map->character_map[y][x] != NULL));


  world.cur_map->character_map[y][x] = c;
  c->x = x;
  c->y = y;
  c->next_turn = 0;
  c->direction = 0;
  c->defeated = 0;

  Pokemon p = spawn_pokemon();
  c->pokemon.push_back(p);

  bool quit = false;
  int counter = 0;
  while (!quit){
    int random = rand() % 100;
    if (random < 60 && counter < 6){
      p = spawn_pokemon();
      c->pokemon.push_back(p);
      counter++;
    } else {
      quit = true;
    }
  }

}

Character *spawn_random_trainer(){
  Character *c = new NPC();
    int random = rand() % 6;
    if (random == 0){
      c->type = char_hiker;
    } else if (random == 1){
      c->type = char_rival;
    } else if (random == 2){
      c->type = char_pacer;
    } else if (random == 3){
      c->type = char_wanderer;
    } else if (random == 4){
      c->type = char_sentry;
    } else {
      c->type = char_explorer;
    }
    spawn_trainer(c);
    return c;
}

void move_hiker(Character *c){
  int best_x = c->x;
  int best_y = c->y;
  int min = INT_MAX;

  for (int dy = -1; dy <= 1; dy++){
    for (int dx = -1; dx <= 1; dx++){
      if (dx == 0 && dy == 0){
        continue;
      }
      int nx = c->x + dx;
      int ny = c->y + dy;

      if (ny < 0 || ny >= MAP_Y || nx < 0 || nx >= MAP_X)
    continue;

      if (world.hiker_dist[ny][nx] < min){
        min = world.hiker_dist[ny][nx];
        best_x = nx;
        best_y = ny;
      }
    }
  }

  world.cur_map->character_map[c->y][c->x] = NULL;
  c->x = best_x;
  c->y = best_y;
  world.cur_map->character_map[c->y][c->x] = c;
  c->next_turn = world_time + move_cost[1][world.cur_map->map[c->y][c->x]];

}


void move_rival(Character *c){
  int best_x = c->x;
  int best_y = c->y;
  int min = INT_MAX;

  for (int dy = -1; dy <= 1; dy++){
    for (int dx = -1; dx <= 1; dx++){
      if (dx == 0 && dy == 0){
        continue;
      }
      int nx = c->x + dx;
      int ny = c->y + dy;

      if (ny < 0 || ny >= MAP_Y || nx < 0 || nx >= MAP_X)
    continue;

      if (world.rival_dist[ny][nx] < min){
        min = world.rival_dist[ny][nx];
        best_x = nx;
        best_y = ny;
      }
    }
  }

  world.cur_map->character_map[c->y][c->x] = NULL;
  c->x = best_x;
  c->y = best_y;
  world.cur_map->character_map[c->y][c->x] = c;
  c->next_turn = world_time + move_cost[2][world.cur_map->map[c->y][c->x]];

}

void move_pacer(Character *c){
  int random = rand() % 4;
  if (c->direction == 0){
    if (random == 0){
    c->direction = 1; // north
  } else if (random == 1){
    c->direction = -1; // south
  } else if (random == 2){
    c->direction = 2; // east
  } else {
    c->direction = -2; // west
  }
  }
  int dx, dy;
  switch (c->direction){
    case -1:
      dx = 0;
      dy = -1;
      break;
    case 1:
      dx = 0;
      dy = 1;
      break;
    case 2:
      dx = 1;
      dy = 0;
      break;
    case -2:
      dx = -1;
      dy = 0;
      break;
  }
  int nx = c->x + dx;
  int ny = c->y + dy;

  if (ny < 0 || ny >= MAP_Y || nx < 0 || nx >= MAP_X ||
      move_cost[2][world.cur_map->map[ny][nx]] == DIJKSTRA_PATH_MAX ||
      world.cur_map->character_map[ny][nx] != NULL){
    c->direction *= -1;
  }
  else{
    world.cur_map->character_map[c->y][c->x] = NULL;
    c->x = nx;
    c->y = ny;
    world.cur_map->character_map[c->y][c->x] = c;
  }
  c->next_turn = world_time + move_cost[2][world.cur_map->map[c->y][c->x]];
}

void move_wanderer(Character *c){
  terrain_type_t current_terrain = world.cur_map->map[c->y][c->x];
  int dx = 0, dy = 0;
  int ny, nx;
  do{
    int random = rand() % 8;
    switch (random){
      case 0:
        dx = -1; dy = -1; // northwest
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 1:
        dx = 0; dy = -1; // north
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 2:
        dx = 1; dy = -1; // northeast
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 3:
        dx = -1; dy = 0; // west
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 4:
        dx = 1; dy = 0; // east
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 5:
        dx = -1; dy = 1; // southwest
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 6:
        dx = 0; dy = 1; // south
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 7:
        dx = 1; dy = 1; // southeast
        ny = c->y + dy;
        nx = c->x + dx;
        break;
    }
  } while ((ny < 0 || ny >= MAP_Y || nx < 0 || nx >= MAP_X ||
      move_cost[2][world.cur_map->map[ny][nx]] == DIJKSTRA_PATH_MAX ||
      world.cur_map->character_map[ny][nx] != NULL || world.cur_map->map[ny][nx] != current_terrain));
  
  world.cur_map->character_map[c->y][c->x] = NULL;
  c->x = nx;
  c->y = ny;
  world.cur_map->character_map[c->y][c->x] = c;
  
   c->next_turn = world_time + move_cost[2][world.cur_map->map[c->y][c->x]];
}

void move_explorer(Character *c){
  int dx = 0, dy = 0;
  int ny, nx;
  do{
    int random = rand() % 8;
    switch (random){
      case 0:
        dx = -1; dy = -1; // northwest
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 1:
        dx = 0; dy = -1; // north
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 2:
        dx = 1; dy = -1; // northeast
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 3:
        dx = -1; dy = 0; // west
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 4:
        dx = 1; dy = 0; // east
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 5:
        dx = -1; dy = 1; // southwest
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 6:
        dx = 0; dy = 1; // south
        ny = c->y + dy;
        nx = c->x + dx;
        break;
      case 7:
        dx = 1; dy = 1; // southeast
        ny = c->y + dy;
        nx = c->x + dx;
        break;
    }
  } while ((ny < 0 || ny >= MAP_Y || nx < 0 || nx >= MAP_X ||
      move_cost[2][world.cur_map->map[ny][nx]] == DIJKSTRA_PATH_MAX ||
      world.cur_map->character_map[ny][nx] != NULL));
  
  world.cur_map->character_map[c->y][c->x] = NULL;
  c->x = nx;
  c->y = ny;
  world.cur_map->character_map[c->y][c->x] = c;

   c->next_turn = world_time + move_cost[2][world.cur_map->map[c->y][c->x]];
}

void init_npcs(heap_t *turn_heap){
  while(turn_heap->size > 0){
    heap_remove_min(turn_heap);
  }

  if (num_trainers >= 2){
    Character *h = new NPC();
    h->type = char_hiker;
    h->defeated = 0;
    spawn_trainer(h);
    heap_insert(turn_heap, h);

    Character *r = new NPC();
    r->type = char_rival;
    r->defeated = 0;
    spawn_trainer(r);
    heap_insert(turn_heap, r);

    for (int i = 0; i < num_trainers - 2; i++){
      Character *temp = spawn_random_trainer();
      heap_insert(turn_heap, temp);
    }
  }
  else{
    Character *temp = spawn_random_trainer();
    heap_insert(turn_heap, temp);
  }

}

void clear_battle_screen(){
  for (int i = 0; i < MAP_Y; i++){
    mvprintw(BATTLE_START_ROW + i, 0, "%*s", MAP_X, " ");
  }
  refresh();
}

int init_wild_pokemon_battle(Pokemon &wild_poke){
  clear_battle_screen();
  refresh();
  mvprintw(BATTLE_START_ROW, 0, "A wild %s appeared! (Level %d, HP: %d)", wild_poke.identifier, wild_poke.level, wild_poke.hp);
  refresh();

  bool is_captured = false;
 
  while (!is_captured){
    mvprintw(BATTLE_START_ROW + 1, 0, "Would you like to (1) Open your bag, or (2) Run?\n");
    refresh();
    int choice = getch();
    switch(choice){
      case '1': {
        mvprintw(BATTLE_START_ROW + 2, 0, "Choose an item to use:");
        mvprintw(BATTLE_START_ROW + 3, 0, "1. Poke Ball (%d) 2. Revive (%d) 3. Potion (%d)", world.pc.bag.num_pokeballs, world.pc.bag.num_revives, world.pc.bag.num_potions);
        int item_choice = getch() - '0';
        if (item_choice == 1){
          if (world.pc.bag.num_pokeballs > 0){
            world.pc.bag.num_pokeballs--;
            if (world.pc.pokemon.size() >= 6){
              mvprintw(BATTLE_START_ROW + 4, 0, "You cannot capture more than 6 Pokemon! You need to switch out a Pokemon before you can capture this one.");
              refresh();
            } else {
              world.pc.pokemon.push_back(wild_poke);
              mvprintw(BATTLE_START_ROW + 4, 0, "Congratulations! You captured %s!", wild_poke.identifier);
              refresh();
              is_captured = true;
            }
          }
      }
      else if (item_choice == 2){
        if (world.pc.bag.num_revives > 0){
          world.pc.bag.num_revives--;
          mvprintw(BATTLE_START_ROW + 4, 0, "Which Pokemon would you like to use the Revive on?");
          for (int i = 0; i < world.pc.pokemon.size(); i++){
            mvprintw(BATTLE_START_ROW + 5 + i, 0, "%d. %s (Level %d, HP: %d)", i + 1, world.pc.pokemon[i].identifier, world.pc.pokemon[i].level, world.pc.pokemon[i].current_hp);
          }
          int revive_choice = getch() - '0';
          Pokemon &revive_poke = world.pc.pokemon[revive_choice - 1];
          revive_poke.current_hp = revive_poke.hp / 2;
          mvprintw(BATTLE_START_ROW + 4, 0, "You used a Revive on %s! It has been revived with %d HP!", revive_poke.identifier, revive_poke.current_hp);
          refresh();
        } else {
          mvprintw(BATTLE_START_ROW + 4, 0, "You have no Revives left!");
          refresh();
        }
      }
      else if (item_choice == 3){
        if (world.pc.bag.num_potions > 0){
          world.pc.bag.num_potions--;
          mvprintw(BATTLE_START_ROW + 4, 0, "Which Pokemon would you like to use the Potion on?");
          for (int i = 0; i < world.pc.pokemon.size(); i++){
            mvprintw(BATTLE_START_ROW + 5 + i, 0, "%d. %s (Level %d, HP: %d)", i + 1, world.pc.pokemon[i].identifier, world.pc.pokemon[i].level, world.pc.pokemon[i].current_hp);
          }
          int potion_choice = getch() - '0';
          Pokemon &potion_poke = world.pc.pokemon[potion_choice - 1];
          if (potion_poke.current_hp <= 0){
            mvprintw(BATTLE_START_ROW + 4, 0, "You cannot use a Potion on a fainted Pokemon!");
            refresh();
          } else {
            potion_poke.current_hp = std::min(potion_poke.current_hp + 20, potion_poke.hp);
            mvprintw(BATTLE_START_ROW + 4, 0, "You used a Potion on %s! It has been healed to %d HP!", potion_poke.identifier, potion_poke.current_hp);
            refresh();
          }
        } else {
          mvprintw(BATTLE_START_ROW + 4, 0, "You have no Potions left!");
          refresh();
        }
      }
      else {
        mvprintw(BATTLE_START_ROW + 4, 0, "Invalid choice!");
        refresh();
      }
      break;
    }
    case '2' : {
      mvprintw(BATTLE_START_ROW + 4, 0, "You ran away safely!");
      refresh();
      is_captured = true;
    }
    }
  }
  return 1;
}

void encounter_pokemon(){

  Pokemon p = spawn_pokemon();
  mvprintw(0, 0, "You have encountered a level %d %s! (HP: %d, Attack: %d, Defense: %d, Speed: %d, Sp. Atk: %d, Sp. Def: %d)",
           p.level, p.identifier, p.hp, p.attack, p.defense, p.speed, p.special_attack, p.special_defense);
  mvprintw(1, 0, "Press c to try to capture it or any other key to run...");
  int choice = getch();

  if (choice == 'c'){
    init_wild_pokemon_battle(p);
  }

}

int move_pc(int dx, int dy, heap_t *turn_heap){
  int nx = world.pc.x + dx;
  int ny = world.pc.y + dy;

  if (ny < 0 || ny >= MAP_Y || nx < 0 || nx >= MAP_X){
    int past_x = world.pc.x;
    int past_y = world.pc.y;
    if (dy == -1 && world.pc.y == 0){
          world.cur_idx[dim_y]--;
          if (new_map() == 1) {
            init_npcs(&world.cur_map->turn_heap);
          }
          mvprintw(0, 0, "Loading new map...");
          pathfind(world.cur_map);
          world.pc.x = past_x;
          world.pc.y = MAP_Y - 1;
    }
    else if (dy == 1 && world.pc.y == MAP_Y - 1){
          world.cur_idx[dim_y]++;
          if (new_map() == 1) {
            init_npcs(&world.cur_map->turn_heap);
          }
          pathfind(world.cur_map);
          world.pc.x = past_x;
          world.pc.y = 0;
          
    }
    else if (dx == -1 && world.pc.x == 0){
          world.cur_idx[dim_x]--;
          if (new_map() == 1) {
            init_npcs(&world.cur_map->turn_heap);
          }
          pathfind(world.cur_map);
          world.pc.x = MAP_X - 1;
          world.pc.y = past_y;
          
    }
    else if (dx == 1 && world.pc.x == MAP_X - 1){
          world.cur_idx[dim_x]++;
          if (new_map() == 1) {
            init_npcs(&world.cur_map->turn_heap);
          }
          pathfind(world.cur_map);
          world.pc.x = 0;
          world.pc.y = past_y;
          
    }
    return 1;
  }
  else if (move_cost[0][world.cur_map->map[ny][nx]] == DIJKSTRA_PATH_MAX ||
      world.cur_map->character_map[ny][nx] != NULL){
      return 0;
  }
  world.pc.x = nx;
  world.pc.y = ny;

    if (world.cur_map->map[ny][nx] == ter_grass){
    int encounter_chance = std::rand() % 100;
    if (encounter_chance < 10){
      encounter_pokemon();
    }
  }

    else if(world.cur_map->map[ny][nx] == ter_mart){
      mvprintw(0, 0, "Welcome to the Poke Mart! You can buy items here.");
      mvprintw(1, 0, "Press s to refill your supplies");
      int choice = getch();
      if (choice == 's'){
        world.pc.bag.num_pokeballs = 10;
        world.pc.bag.num_potions = 5;
        world.pc.bag.num_revives = 3;
      }
      refresh();
    }

    else if(world.cur_map->map[ny][nx] == ter_center){
      mvprintw(0, 0, "Welcome to the Pokemon Center!");
      mvprintw(1, 0, "Press s to heal your Pokemon");
      int choice = getch();
      if (choice == 's'){
        for (Pokemon &p : world.pc.pokemon){
          p.current_hp = p.hp;
        }
      }
    }
      refresh();

  return 1;
}


int game_turn(heap_t *turn_heap){
  Character *c = (Character *) heap_remove_min(turn_heap);
  world_time = c->next_turn;
  if (c->defeated){
    return 1;
  }
  switch (c->type){
    case char_hiker:
      move_hiker(c);
      break;
    case char_rival:
      move_rival(c);
      break;
    case char_pacer:
      move_pacer(c);
      break;
    case char_wanderer:
      move_wanderer(c);
      break;
    case char_sentry:
      c->next_turn = world_time + world.rival_dist[c->y][c->x];
      break;
    case char_explorer:
      move_explorer(c);
      break;
  }
  heap_insert(turn_heap, c);
  return 0;
}

int get_input_nonblocking(char *c)
{
  struct timeval tv = {0, 0};  // no waiting
  fd_set fds;

  FD_ZERO(&fds);
  FD_SET(STDIN_FILENO, &fds);

  if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
    return scanf(" %c", c);
  }

  return 0;  // no input
}

int print_pokemon(Character *c){
  WINDOW *win = newwin(10, 50, 1, 1);
  box(win, 0, 0);
  mvwprintw(win, 1, 1, "Pokemon:");
  size_t i;
  for (i = 0; i < c->pokemon.size(); i++){
    Pokemon &p = c->pokemon[i];
    mvwprintw(win, i + 2, 2, "%d. %s (Level %d)", i + 1, p.identifier, p.level);
  }
  mvwprintw(win, i + 2, 2, "Press any key to escape...");
  wrefresh(win);
  getch();
  delwin(win);
  return 0;

}

int print_trainers(){
  
  WINDOW *pad = newpad(200, 80);

  scrollok(pad, TRUE);
  keypad(pad, TRUE);
  int line = 0;
  wprintw(pad, "Trainers in the world:\n");
  line++;
  for (int y = 0; y < MAP_Y; y++){
    for (int x = 0; x < MAP_X; x++){
      if ((world.cur_map->character_map[y][x] != NULL) && (x != world.pc.x && y != world.pc.y)){
        int dx = x - world.pc.x;
        int dy = y - world.pc.y;
        const char *ns = (dy < 0) ? "North" : "South";
        const char *ew = (dx < 0) ? "West" : "East";
        Character *c = world.cur_map->character_map[y][x];
        char symbol;
        if (c->type == char_hiker){
          symbol = 'h';
        } else if (c->type == char_rival){
          symbol = 'r';
        } else if (c->type == char_pacer){
          symbol = 'p';
        } else if (c->type == char_wanderer){
          symbol = 'w';
        } else if (c->type == char_sentry){
          symbol = 's';
        } else {
          symbol = 'e';
        }
        wprintw(pad, "Trainer (%c) at (%d,%d) is %d steps %s and %d steps %s of you.\n",
                 symbol, x, y, abs(dx), ew, abs(dy), ns);
        line++;
      }
    }
  }
  wprintw(pad, "Press esc to return to the game.\n");
  int pad_pos = 0;
  int quit = 0;

  prefresh(pad, pad_pos, 0, 1, 0, 20, 79);

  while (!quit){
  int key = getch();
  switch (key){
    case 27:
      quit = 1;
      break;
    case KEY_UP:
      if (pad_pos > 0){
        pad_pos--;
      }
      break;
    case KEY_DOWN:
      if (pad_pos + 20 < line){
        pad_pos++;
      }
      break;
  }
  prefresh(pad, pad_pos, 0, 1, 0, 20, 79);
}
  delwin(pad);
  clear();
  refresh();
  return 0;

}

Character *check_npc_around_pc(){
  for (int dy = -1; dy <= 1; dy++){
    for (int dx = -1; dx <= 1; dx++){
       int nx = world.pc.x + dx;
       int ny = world.pc.y + dy;

      if (ny < 0 || ny >= MAP_Y || nx < 0 || nx >= MAP_X)
    continue;

      if (world.cur_map->character_map[ny][nx]){
        refresh();
        return world.cur_map->character_map[ny][nx];
      }
    }
  }
  return NULL;
}

int calculate_damage(Pokemon *attacker, Pokemon *defender, Move &move){
int base = (((2 * attacker->level) / 5 + 2) * move.power * attacker->attack / defender->defense) / 50 + 2;

int critical_chance = std::rand() % 256;
bool is_critical = false;
if (critical_chance < pokemon_stats_map[attacker->id].base_stat / 2){ 
  is_critical = true;
}
double critical = is_critical ? 1.5 : 1.0;
double random = (rand() % 16 + 85) / 100.0;
double stab = (move.type_id == pokemon_type_map[attacker->id]) ? 1.5 : 1.0;
double type = 1.0;

int damage = (int)(base * critical * random * stab * type);

return damage;
}

int npc_attack(Trainer_Battle &battle, Move npc_move){
  int rand = std::rand() % 100;
  if (rand > npc_move.accuracy){
    mvprintw(BATTLE_START_ROW + 2, 0, "The opponent's %s used %s but missed!", battle.npc->pokemon[battle.npc_pokemon_index].identifier, npc_move.identifier);
    mvprintw(BATTLE_START_ROW + 3, 0, "Press any key to continue...");
    refresh();
    getch();
    mvprintw(BATTLE_START_ROW + 2, 0, "%*s", MAP_X, " ");\
    return 1;
  }
  Pokemon *npc_poke = &battle.npc->pokemon[battle.npc_pokemon_index];
  Pokemon *pc_poke = battle.pc_pokemon;
  int damage = calculate_damage(npc_poke, pc_poke, npc_move);
            pc_poke->current_hp -= damage;
            mvprintw(BATTLE_START_ROW + 2, 0, "The opponent's %s used %s and dealt %d damage!", npc_poke->identifier, npc_move.identifier, damage);
            mvprintw(BATTLE_START_ROW + 3, 0, "Press any key to continue...");
            refresh();
            getch();
            mvprintw(BATTLE_START_ROW + 2, 0, "%*s", MAP_X, " ");
            mvprintw(BATTLE_START_ROW + 3, 0, "%*s", MAP_X, " ");
            if (pc_poke->current_hp <= 0){
              pc_poke->current_hp = 0;
              mvprintw(BATTLE_START_ROW + 7, 0, "Your %s fainted!", pc_poke->identifier);
              refresh();
              bool has_usable_pokemon = false;
              for (Pokemon &p : world.pc.pokemon){
                if (p.current_hp > 0){
                  has_usable_pokemon = true;
                  break;
                }
              }
              if (has_usable_pokemon == false){
                mvprintw(BATTLE_START_ROW + 8, 0, "You were defeated...");
                refresh();
                battle.is_battle_over = true;
                return 1;
              }
              else {
                mvprintw(BATTLE_START_ROW + 4, 0, "Choose a Pokemon to switch in:");
                for (int i = 0; i < world.pc.pokemon.size(); i++){
                  mvprintw(BATTLE_START_ROW + 5 + i, 0, "%d. %s (Level %d, HP: %d)", i + 1, world.pc.pokemon[i].identifier, world.pc.pokemon[i].level, world.pc.pokemon[i].current_hp);
                }
                while(pc_poke->current_hp <= 0){
                  int switch_choice = getch() - '0';
                  if (world.pc.pokemon[switch_choice - 1].current_hp <= 0){
                    mvprintw(BATTLE_START_ROW + 7, 0, "You cannot switch to a fainted Pokemon!");
                    refresh();
                    battle.is_battle_over = true;
                    return 1;
                  } else {
                    Pokemon temp = world.pc.pokemon[0];
                    world.pc.pokemon[0] = world.pc.pokemon[switch_choice - 1];
                    world.pc.pokemon[switch_choice - 1] = temp;
                    pc_poke = &world.pc.pokemon[0];
                    mvprintw(BATTLE_START_ROW + 4, 0, "Go %s! (Level %d, HP: %d)", pc_poke->identifier, pc_poke->level, pc_poke->current_hp); 
                    refresh();
                  }
              }
              mvprintw(BATTLE_START_ROW + 4, 0, "%*s", MAP_X, " ");
            }
          }
    return 1;
}

int pc_attack(Trainer_Battle &battle, Move move, Pokemon *pc_poke, Pokemon *npc_poke){
  int rand = std::rand() % 100;
  if (rand > move.accuracy){
    mvprintw(BATTLE_START_ROW + 2, 0, " %s used %s but missed!", pc_poke->identifier, move.identifier);
    mvprintw(BATTLE_START_ROW + 3, 0, "Press any key to continue...");
    refresh();
    getch();
    mvprintw(BATTLE_START_ROW + 2, 0, "%*s", MAP_X, " ");\
    return 1;
  }
  int damage = calculate_damage(pc_poke, npc_poke, move);
            npc_poke->current_hp -= damage;
            mvprintw(BATTLE_START_ROW + 6, 0, "Your %s used %s and dealt %d damage!", pc_poke->identifier, move.identifier, damage);
            mvprintw(BATTLE_START_ROW + 7, 0, "Press any key to continue...");
            refresh();
            getch();
            mvprintw(BATTLE_START_ROW + 7, 0, "%*s", MAP_X, " ");
            mvprintw(BATTLE_START_ROW + 6, 0, "%*s", MAP_X, " ");
            if (npc_poke->current_hp <= 0){
              npc_poke->current_hp = 0;
              mvprintw(BATTLE_START_ROW + 7, 0, "The opponent's %s fainted!", npc_poke->identifier);
              refresh();
              battle.npc_pokemon_index++;
              if (battle.npc_pokemon_index >= battle.npc->pokemon.size()){
                clear_battle_screen();
                mvprintw(BATTLE_START_ROW + 8, 0, "You defeated the opponent!");
                mvprintw(BATTLE_START_ROW + 9, 0, "Press any key to continue...");
                getch();
                refresh();
                battle.is_battle_over = true;
                battle.npc->defeated = 1;
                return 1;
              } else {
                npc_poke = &battle.npc->pokemon[battle.npc_pokemon_index];
                mvprintw(BATTLE_START_ROW, 0, "The opponent sent out %s! (Level %d, HP: %d)", npc_poke->identifier, npc_poke->level, npc_poke->current_hp);
                refresh();
              }
            }
    return 1;
}

int init_trainer_battle(Character *t){

  clear_battle_screen();
  refresh();
  int npc_pokemon_index = 0;

  Trainer_Battle battle(t, npc_pokemon_index, &world.pc.pokemon[0] );

  while (!battle.is_battle_over){
    Pokemon *npc_poke = &t->pokemon[npc_pokemon_index];
    Pokemon *pc_poke = &world.pc.pokemon[0];
    while(world.pc.pokemon[0].current_hp <= 0){
      int check_index = 0;
      if (check_index >= world.pc.pokemon.size()){
        mvprintw(BATTLE_START_ROW + 8, 0, "You have no more usable Pokemon! You were defeated...");
        refresh();
        battle.is_battle_over = true;
        return 1;
      }
    }
    mvprintw(BATTLE_START_ROW, 0, "Would you like to (1) Fight, (2) Open Bag, (3) Run, or (4) Switch Pokemon?\n");
    int choice = getch();
    switch (choice){
      case '1': {
        mvprintw(BATTLE_START_ROW, 0, "%*s", MAP_X, " ");
        mvprintw(BATTLE_START_ROW + 4, 0, "Go %s! (Level %d, HP: %d)", pc_poke->identifier, pc_poke->level, pc_poke->current_hp); 
        mvprintw(BATTLE_START_ROW + 1, 0, "The opponent sent out %s! (Level %d, HP: %d)", npc_poke->identifier, npc_poke->level, npc_poke->current_hp);
        refresh();
          mvprintw(BATTLE_START_ROW + 5, 0, "Choose a move:");
          for (int i = 0; i < pc_poke->moves.size(); i++){
            mvprintw(BATTLE_START_ROW + 6 + i, 0, "%d. %s (PP: %d)", i + 1, pc_poke->moves[i].identifier, pc_poke->moves[i].pp);
          }
          int move_choice = getch() - '0';
          for (int i = BATTLE_START_ROW + 5; i < BATTLE_START_ROW + 6 + pc_poke->moves.size(); i++){
            mvprintw(i, 0, "%*s", MAP_X, " ");
          }
          Move move = pc_poke->moves[move_choice - 1];
          int npc_move_rand = rand() % npc_poke->moves.size();
          Move npc_move = npc_poke->moves[npc_move_rand];
          if (npc_move.priority > move.priority || (npc_move.priority == move.priority && npc_poke->speed > pc_poke->speed)){
            npc_attack(battle, npc_move);
            if(!battle.is_battle_over){
              pc_attack(battle, move, pc_poke, npc_poke);
            }
          } else if (move.priority == npc_move.priority && pc_poke->speed == npc_poke->speed){
             int coin_flip = rand() % 2;
              if (coin_flip == 0){
                npc_attack(battle, npc_move);
                if(!battle.is_battle_over){
                  pc_attack(battle, move, pc_poke, npc_poke);
                }
              } else {
                pc_attack(battle, move, pc_poke, npc_poke);
                if(!battle.is_battle_over){
                  npc_attack(battle, npc_move);
                }
              }
          } else {
             pc_attack(battle, move, pc_poke, npc_poke);
              if(!battle.is_battle_over){
                  npc_attack(battle, npc_move);
                }
          }
          refresh();
          break;
        }
      case '4':{
        mvprintw(BATTLE_START_ROW + 4, 0, "Choose a Pokemon to switch to:");
        for (int i = 0; i < world.pc.pokemon.size(); i++){
          mvprintw(BATTLE_START_ROW + 5 + i, 0, "%d. %s (Level %d, HP: %d)", i + 1, world.pc.pokemon[i].identifier, world.pc.pokemon[i].level, world.pc.pokemon[i].current_hp);
        }
        int switch_choice = getch() - '0';
        if (world.pc.pokemon[switch_choice - 1].current_hp <= 0){
          mvprintw(BATTLE_START_ROW + 7, 0, "You cannot switch to a fainted Pokemon!");
          refresh();
        } else {
          Pokemon temp = world.pc.pokemon[0];
          world.pc.pokemon[0] = world.pc.pokemon[switch_choice - 1];
          world.pc.pokemon[switch_choice - 1] = temp;
          pc_poke = &world.pc.pokemon[0];
          refresh();
        }
        mvprintw(BATTLE_START_ROW + 5, 0, "Press any key to continue...");
        getch();
        clear_battle_screen();
        npc_attack(battle, npc_poke->moves[rand() % npc_poke->moves.size()]);
        break;
      }
      case '3' : {
        mvprintw(BATTLE_START_ROW + 4, 0, "You can't run away from a trainer battle!");
        mvprintw(BATTLE_START_ROW + 5, 0, "Press any key to continue...");
        refresh();
        getch();
        clear_battle_screen();
        break;
      }
      case '2': {
        clear_battle_screen();
        mvprintw(BATTLE_START_ROW, 0, "Choose an item to use:");
        mvprintw(BATTLE_START_ROW + 1, 0, "1. Revive (%d), 2. Potion(%d)", world.pc.bag.num_revives, world.pc.bag.num_potions);
        int item_choice = getch() - '0';
        if(item_choice == 1){
          if (world.pc.bag.num_revives > 0){
            world.pc.bag.num_revives--;
            mvprintw(BATTLE_START_ROW + 4, 0, "Choose a Pokemon to revive:");
            for (int i = 0; i < world.pc.pokemon.size(); i++){
              mvprintw(BATTLE_START_ROW + 5 + i, 0, "%d. %s (Level %d, HP: %d)", i + 1, world.pc.pokemon[i].identifier, world.pc.pokemon[i].level, world.pc.pokemon[i].current_hp);
              }
            int revive_choice = getch() - '0';
            Pokemon &revive_poke = world.pc.pokemon[revive_choice - 1];
            revive_poke.current_hp = revive_poke.hp / 2;
            mvprintw(BATTLE_START_ROW + 4, 0, "You used a Revive on %s! It has been revived with %d HP!", revive_poke.identifier, revive_poke.current_hp);
            refresh();
          } else {
            mvprintw(BATTLE_START_ROW + 4, 0, "You have no Revives left!");
            refresh();
          }
        }
        else if (item_choice == 2){
          if (pc_poke->current_hp <= 0){
            mvprintw(BATTLE_START_ROW + 4, 0, "You cannot use a Potion on a fainted Pokemon!");
            refresh();
          }
          else{
            if (world.pc.bag.num_potions > 0){
              world.pc.bag.num_potions--;
              pc_poke->current_hp = std::min(pc_poke->current_hp + 20, pc_poke->hp);
              mvprintw(BATTLE_START_ROW + 4, 0, "You used a Potion on %s! It has been healed to %d HP!", pc_poke->identifier, pc_poke->current_hp);
              refresh();
            } else {
              mvprintw(BATTLE_START_ROW + 4, 0, "You have no Potions left!");
              refresh();
            }
          }
        }
      else{
        mvprintw(BATTLE_START_ROW + 4, 0, "Invalid choice!");
        refresh();
      }
      mvprintw(BATTLE_START_ROW + 5, 0, "Press any key to continue...");
      getch();
      clear_battle_screen();
      refresh();
      npc_attack(battle, npc_poke->moves[rand() % npc_poke->moves.size()]);
      break;
    }
  }
  }
  return 1;
}

class CSVData{
public:
  virtual ~CSVData() = default;

  virtual void parseLine(const std::string& line) = 0;

  template <typename T>
  static std::vector<T> parseCSV(const char* filename) {
    std::vector<T> data;
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
      std::cerr << "Could not open file: " << filename << std::endl;
      return data;
    }
    std::getline(file, line); // Skip header

    while (std::getline(file, line)) {
      T item;
      item.parseLine(line);
      data.push_back(item);
    }
    return data;
  }
};

void parse_and_print_pokemon_data(const char *name) {
    char path[512];
    
    snprintf(path, sizeof(path), "/share/cs327/pokedex/pokedex/data/csv/%s.csv", name);
    FILE *file = fopen(path, "r");
    if (!file) {
        char *home = getenv("HOME");
        snprintf(path, sizeof(path), "%s/.poke327/pokedex/pokedex/data/csv/%s.csv", home, name);
        file = fopen(path, "r");
    }
    if (strcmp(name, "pokemon") == 0) {
        std::vector<Pokemon> pokemon_data = CSVData::parseCSV<Pokemon>(path);
        for (const auto& p : pokemon_data) {
            printf("ID: %d, Identifier: %s, Height: %d, Weight: %d, Base Experience: %d, Order: %d, Is Default: %d\n",
                   p.id, p.identifier, p.height, p.weight, p.base_experience, p.order, p.is_default);
        }
    } else if (strcmp(name, "moves") == 0) {
        std::vector<Move> moves_data = CSVData::parseCSV<Move>(path);
        for (const auto& m : moves_data) {
            printf("ID: %d, Identifier: %s, Generation ID: %d, Type ID: %d, Power: %d, PP: %d, Accuracy: %d, Priority: %d\n",
                   m.id, m.identifier, m.generation_id, m.type_id, m.power, m.pp, m.accuracy, m.priority);
        }
    } else if (strcmp(name, "pokemon_moves") == 0) {
        std::vector<PokemonMove> pokemon_moves_data = CSVData::parseCSV<PokemonMove>(path);
        for (const auto& pm : pokemon_moves_data) {
            printf("Pokemon ID: %d, Version Group ID: %d, Move ID: %d, Pokemon Move Method ID: %d, Level: %d, Order: %d\n",
                   pm.pokemon_id, pm.version_group_id, pm.move_id, pm.pokemon_move_method_id, pm.level, pm.order);
        }
    } else if (strcmp(name, "pokemon_species") == 0) {
        std::vector<PokemonSpecies> pokemon_species_data = CSVData::parseCSV<PokemonSpecies>(path);
        for (const auto& ps : pokemon_species_data) {
            printf("ID: %d, Identifier: %s, Generation ID: %d\n", ps.id, ps.identifier, ps.generation_id);
        }
    } else if (strcmp(name, "experience") == 0) {
        std::vector<Experience> experience_data = CSVData::parseCSV<Experience>(path);
        for (const auto& e : experience_data) {
            std::cout << "Growth Rate ID: " << int_or_na(e.growth_rate_id) << ", Level: " << int_or_na(e.level) << ", Experience: " << int_or_na(e.experience) << std::endl;
        }
    } else if (strcmp(name, "type_names") == 0){
      std::vector<TypeName> type_names_data = CSVData::parseCSV<TypeName>(path);
      for (const auto& tn : type_names_data) {
          std::cout << "Type ID: " << int_or_na(tn.type_id) << ", Name: " << tn.name << std::endl;
      }
    } else if (strcmp(name, "pokemon_stats") == 0){
      std::vector<PokemonStat> pokemon_stats_data = CSVData::parseCSV<PokemonStat>(path);
      for (const auto& ps : pokemon_stats_data) {
          std::cout << "Pokemon ID: " << int_or_na(ps.pokemon_id) << ", Stat ID: " << int_or_na(ps.stat_id) << ", Base Stat: " << int_or_na(ps.base_stat) << ", Effort: " << int_or_na(ps.effort) << std::endl;
      }
    } else if (strcmp(name, "pokemon_types") == 0){
      std::vector<PokemonTypes> pokemon_types_data = CSVData::parseCSV<PokemonTypes>(path);
      for (const auto& pt : pokemon_types_data) {
          std::cout << "Pokemon ID: " << int_or_na(pt.pokemon_id) << ", Type ID: " << int_or_na(pt.type_id) << ", Slot: " << int_or_na(pt.slot) << std::endl;
      }
    } else if (strcmp(name, "stats") == 0){
      std::vector<Stats> stats_data = CSVData::parseCSV<Stats>(path);
      for (const auto& s : stats_data) {
          std::cout << "ID: " << int_or_na(s.id) << ", Identifier: " << s.identifier << ", Is Battle Only: " << int_or_na(s.is_battle_only) << ", Game Index: " << int_or_na(s.game_index) << std::endl;
      }
    } else {
        fprintf(stderr, "Unknown data type: %s\n", name);
    }
}

void parse_pokemon_data() {
    char path[512];
    const char *accept[] = {"pokemon", "moves", "pokemon_moves", "pokemon_species", "experience",
                       "type_names", "pokemon_stats", "stats", "pokemon_types"};
    char name[50];
    for (size_t i = 0; i < sizeof(accept) / sizeof(accept[0]); i++) {
        strcpy(name, accept[i]);
        snprintf(path, sizeof(path), "/share/cs327/pokedex/pokedex/data/csv/%s.csv", name);
        FILE *file = fopen(path, "r");
        if (!file) {
            char *home = getenv("HOME");
        snprintf(path, sizeof(path), "%s/.poke327/pokedex/pokedex/data/csv/%s.csv", home, name);
        file = fopen(path, "r");
    }
    if (strcmp(name, "pokemon") == 0) {
        pokemon_data = CSVData::parseCSV<Pokemon>(path);
        for (const auto& p : pokemon_data) {
            pokemon_map[p.id] = p;
        }
    } else if (strcmp(name, "moves") == 0) {
        std::vector<Move> moves_data = CSVData::parseCSV<Move>(path);
        for (const auto& m : moves_data) {
            move_map[m.id] = m;
        }
    } else if (strcmp(name, "pokemon_moves") == 0) {
        std::vector<PokemonMove> pokemon_moves_data = CSVData::parseCSV<PokemonMove>(path);
        for (const auto& pm : pokemon_moves_data) {
            moves_by_pokemon[pm.pokemon_id].push_back(pm);
        }
    } else if (strcmp(name, "pokemon_species") == 0) {
        std::vector<PokemonSpecies> pokemon_species_data = CSVData::parseCSV<PokemonSpecies>(path);
        /*for (const auto& ps : pokemon_species_data) {
           // pokemon_species_map[ps.id] = ps;
        }*/
    } else if (strcmp(name, "experience") == 0) {
        std::vector<Experience> experience_data = CSVData::parseCSV<Experience>(path);
        for (const auto& e : experience_data) {
            std::cout << "Growth Rate ID: " << int_or_na(e.growth_rate_id) << ", Level: " << int_or_na(e.level) << ", Experience: " << int_or_na(e.experience) << std::endl;
        }
    } else if (strcmp(name, "type_names") == 0){
      std::vector<TypeName> type_names_data = CSVData::parseCSV<TypeName>(path);
      for (const auto& tn : type_names_data) {
          std::cout << "Type ID: " << int_or_na(tn.type_id) << ", Name: " << tn.name << std::endl;
      }
    } else if (strcmp(name, "pokemon_stats") == 0){
      std::vector<PokemonStat> pokemon_stats_data = CSVData::parseCSV<PokemonStat>(path);
      for (const auto& ps : pokemon_stats_data) {
          pokemon_stats_map[ps.pokemon_id] = ps;
      }
    } else if (strcmp(name, "pokemon_types") == 0){
      std::vector<PokemonTypes> pokemon_types_data = CSVData::parseCSV<PokemonTypes>(path);
      for (const auto& pt : pokemon_types_data) {
          pokemon_type_map[pt.pokemon_id] = pt.type_id;
      }
    } else if (strcmp(name, "stats") == 0){
      std::vector<Stats> stats_data = CSVData::parseCSV<Stats>(path);
      /*for (const auto& s : stats_data) {
         // std::cout << "ID: " << int_or_na(s.id) << ", Identifier: " << s.identifier << ", Is Battle Only: " << int_or_na(s.is_battle_only) << ", Game Index: " << int_or_na(s.game_index) << std::endl;
      }*/
    } else {
        fprintf(stderr, "Unknown data type: %s\n", name);
    }
  }
}

int pick_pokemon(){
  mvprintw(0, 0, "Choose a Pokemon to battle with:\n");
  Pokemon p1 = spawn_pokemon();
  Pokemon p2 = spawn_pokemon();
  Pokemon p3 = spawn_pokemon();

  mvprintw(1, 0, "1. %s (Level %d)\n", p1.identifier, p1.level);
  mvprintw(2, 0, "2. %s (Level %d)\n", p2.identifier, p2.level);
  mvprintw(3, 0, "3. %s (Level %d)\n", p3.identifier, p3.level);
  mvprintw(4, 0, "Enter the number of the Pokemon you want to choose: ");
  refresh();
  while (true){
    int choice = getch();
    switch (choice) {
      case '1':
        world.pc.pokemon.push_back(p1);
        return 1;
      case '2':
        world.pc.pokemon.push_back(p2);
        return 1;
      case '3':
        world.pc.pokemon.push_back(p3);
        return 1;
      default:
        mvprintw(5, 0, "Invalid choice. Try again.");
        getch();
        refresh();
  }
}
}

int main(int argc, char *argv[])
{
  int moved = 1;

  if (argc == 2){
    const char *accept[] = {"pokemon", "moves", "pokemon_moves", "pokemon_species", "experience",
                       "type_names", "pokemon_stats", "stats", "pokemon_types"};

    for (int i = 0; i < 9; i++){
      if (strcmp(argv[1], accept[i]) == 0){
        parse_and_print_pokemon_data(argv[1]);
        return 0;
      }
    }
}
else{
  parse_pokemon_data();
}

  initscr();
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
  init_pair(3, COLOR_YELLOW, COLOR_BLUE);
  init_pair(4, COLOR_CYAN, COLOR_BLACK);
  init_pair(5, COLOR_WHITE, COLOR_BLACK);
  init_pair(6, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(7, COLOR_BLUE, COLOR_BLACK);

  noecho();
  cbreak();
  keypad(stdscr, TRUE);
  curs_set(0);
  struct timeval tv;
  uint32_t seed;

  if (argc == 2) {
    errno = 0;
    seed = strtol(argv[1], NULL, 10);
    if (!isdigit(argv[1][0]) || errno) {
      mvprintw(0, 0, "Invalid seed value on command line.\n");
      return -1;
    }
  } else {
    gettimeofday(&tv, NULL);
    seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
  }

  mvprintw(0, 0, "Using seed: %u\n", seed);
  srand(seed);

  init_world();

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--numtrainers") == 0) {
        if (i + 1 < argc) {
            num_trainers = atoi(argv[i + 1]);
            i++; // skip the number
        } else {
            fprintf(stderr, "--numtrainers requires a value\n");
            exit(1);
        }
    }
}

  init_npcs(&world.cur_map->turn_heap);

  init_pc();
  world.pc.bag.num_pokeballs = 10;
  world.pc.bag.num_potions = 5;
  world.pc.bag.num_revives = 2;

  pathfind(world.cur_map);
  
  pick_pokemon();

  int quit = 0;

  while (!quit) {
    print_map();  
    refresh();
    Character *npc = check_npc_around_pc();
    if (npc && !npc->defeated){
        mvprintw(0, 0, "A trainer has challenged you to a battle! Press q to battle, w to see their pokemon, or any other key to ignore: \n");
        int battle_input = getch();
        if (battle_input == 'q' ){
         init_trainer_battle(npc);
        }
        else if (battle_input == 'w'){
          print_pokemon(npc);
        }
        refresh();
    }
    else if (!moved){
      mvprintw(0, 0, "There's an obstacle in the way!\n");
      moved = 1;
    }
    else{ 
      mvprintw(0, 0, "Current position is %d%cx%d%c (%d,%d).  "
            "Enter command: ",
             abs(world.cur_idx[dim_x] - (WORLD_SIZE / 2)),
             world.cur_idx[dim_x] - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
             abs(world.cur_idx[dim_y] - (WORLD_SIZE / 2)),
             world.cur_idx[dim_y] - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S',
             world.cur_idx[dim_x] - (WORLD_SIZE / 2),
             world.cur_idx[dim_y] - (WORLD_SIZE / 2));
      }
    int key = getch();
    mvprintw(0, 0, "\n");
      switch (key) {
      case 'Q':
        quit = 1;
        break;
      case 'k': case '8': //up
        if (move_pc(0, -1, &world.cur_map->turn_heap) == 0) {
          moved = 0;
        }
        pathfind(world.cur_map);
      game_turn(&world.cur_map->turn_heap);
      print_map();
      break;
      case 'j': case '2' :
        if (move_pc(0, 1, &world.cur_map->turn_heap) == 0) {
          moved = 0;
        }
        pathfind(world.cur_map);
      game_turn(&world.cur_map->turn_heap);
      print_map();
      break;
      case 'h': case '4':
        if (move_pc(-1, 0, &world.cur_map->turn_heap) == 0) {
          moved = 0;
        }
        pathfind(world.cur_map);
      game_turn(&world.cur_map->turn_heap);
      print_map();
        break;
      case 'l': case '6':
        if (move_pc(1, 0, &world.cur_map->turn_heap) == 0) {
          moved = 0;
        }
        pathfind(world.cur_map);
      game_turn(&world.cur_map->turn_heap);
      print_map();
        break;
      case 'y': case '7':
        if (move_pc(-1, -1, &world.cur_map->turn_heap) == 0) {
          moved = 0;
        }
        pathfind(world.cur_map);
      game_turn(&world.cur_map->turn_heap);
      print_map();
        break;
      case 'u': case '9':
        if (move_pc(1, -1, &world.cur_map->turn_heap) == 0) {
          moved = 0;
        }
        pathfind(world.cur_map);
      game_turn(&world.cur_map->turn_heap);
      print_map();
        break;
      case 'b': case '1':
        if (move_pc(-1, 1, &world.cur_map->turn_heap) == 0) {
          moved = 0;
        }
        pathfind(world.cur_map);
      game_turn(&world.cur_map->turn_heap);
      print_map();
        break;
      case 'n': case '3':
        if (move_pc(1, 1, &world.cur_map->turn_heap) == 0) {
          moved = 0;
        }
        pathfind(world.cur_map);
        game_turn(&world.cur_map->turn_heap);
        print_map();
        break;
      case '>':
      case '5': case ' ':
        pathfind(world.cur_map);
        game_turn(&world.cur_map->turn_heap);
        print_map();
        break;
      case 't':
        print_trainers();
        clear();
        refresh();
        break;
      case 'f':{
        int x;
        int y;
        echo();
        mvprintw(0, 0, "Enter coordinates to fly i.e (50, 50): ");
        scanw((char *) " (%d, %d)", &x, &y);
        noecho();
        if (x >= -200 && x <= 200 && y >= -200  && y <= 200) {
          world.cur_idx[dim_x] = x + 200;
          world.cur_idx[dim_y] = y + 200;
          mvprintw(0, 0, "Flying to (%d, %d)...\n", x, y);
          mvprintw(1, 0, "Press any key to continue.\n");
          getch();
          if (new_map()) {
            init_npcs(&world.cur_map->turn_heap);
          }
          init_pc();
          pathfind(world.cur_map);
        } else {
          mvprintw(0, 0, "Invalid coordinates for flying. Press any key to continue.\n");
          getch();
        }
      }
      case 'B':{
        WINDOW *win = newwin(10, 50, 1, 1);
        box(win, 0, 0);
        mvwprintw(win, 1, 1, "Which item would you like to use?");
        mvwprintw(win, 2, 1, "1. Pokeballs (%d)", world.pc.bag.num_pokeballs);
        mvwprintw(win, 3, 1, "2. Potions (%d)", world.pc.bag.num_potions);
        mvwprintw(win, 4, 1, "3. Revives (%d)", world.pc.bag.num_revives);
        int choice = wgetch(win);
        for (int i = 1; i < 11; i++){
          mvwprintw(win, i, 0, "%*s", MAP_X, " ");
        }
        switch (choice) {
          case '1':{
            mvwprintw(win, 0, 0, "You have %d Pokeballs. Press any key to continue.\n", world.pc.bag.num_pokeballs);
            wrefresh(win);
            getch();
            break;
          }
          case '2':{
            if (world.pc.bag.num_potions > 0){
              world.pc.bag.num_potions--;
            } else {
              mvwprintw(win, 0, 0, "You have no Potions left! Press any key to continue.\n");
              wrefresh(win);
              getch();
            }
            mvwprintw(win, 0, 0, "Which Pokemon would you like to use a Potion on?");
            for (int i = 1; i < world.pc.pokemon.size() + 1; i++){
              mvwprintw(win, i, 1, "%d. %s (HP: %d/%d)", i, world.pc.pokemon[i - 1].identifier, world.pc.pokemon[i - 1].current_hp, world.pc.pokemon[i - 1].hp);
            }
            choice = wgetch(win) - '0';
            Character *pc = &world.pc;
            Pokemon &p = pc->pokemon[choice - 1];
            if (p.hp != 0){
              p.current_hp = std::min(p.current_hp + 20, p.hp);
              mvwprintw(win, 0, 0, "You used a Potion on %s! It has been healed to %d HP! Press any key to continue.\n", p.identifier, p.current_hp);
              wrefresh(win);
              getch();
            }
            else{
              mvwprintw(win, 1, 0, "Incorrect input or you tried to use a potion on a fainted Pokemon. Press any key to continue.\n");
              world.pc.bag.num_potions++;
              wrefresh(win);
              getch();
            }
            break;
          }
          case '3':{
            if (world.pc.bag.num_revives > 0){
              world.pc.bag.num_revives--;
            }
            else{
              mvwprintw(win, 0, 0, "You have no Revives left! Press any key to continue.\n");
              wrefresh(win);
              getch();
              break;
            }
            mvwprintw(win, 0, 0, "Which Pokemon would you like to use a Revive on?");
            for (int i = 1; i < world.pc.pokemon.size() + 1; i++){
              mvwprintw(win, i, 1, "%d. %s (HP: %d/%d)", i, world.pc.pokemon[i - 1].identifier, world.pc.pokemon[i - 1].current_hp, world.pc.pokemon[i - 1].hp);
            }
            choice = wgetch(win) - '0';
            for (int i = 1; i < 11; i++){
              mvwprintw(win, i, 0, "%*s", MAP_X, " ");
             }
            Character *pc = &world.pc;
            Pokemon &poke = pc->pokemon[choice - 1];
            if (poke.hp != 0){
              poke.current_hp = world.pc.pokemon[choice - 1].hp / 2;
              mvwprintw(win, 1, 0, "You used a Revive on %s! It has been healed to %d HP! Press any key to continue.\n", poke.identifier, poke.current_hp);
              wrefresh(win);
              getch();
            }
            else{
              mvwprintw(win, 1, 0, "Incorrect input. Press any key to continue.\n");
              world.pc.bag.num_revives++;
              wrefresh(win);
              getch();
            }
            break;
        }
      }

      }
      default:
        mvprintw(0, 0, "%c: Invalid input.  Enter '?' for help.\n", key);
        break;
      }
      
  }

  delete_world();

  mvprintw(4, 0, "But how are you going to be the very best if you quit?\n");
  endwin();
  return 0;
}
