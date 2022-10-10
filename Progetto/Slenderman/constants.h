#pragma once

#include <unordered_set>

#include "glm/glm.hpp"

// COSTANTI DI CONFIGURAZIONE DEL GIOCO
// -------------------------------------------------------------------------------------------
const unsigned int SCR_WIDTH = 1440;
const unsigned int SCR_HEIGHT = 900;
const char* APP_TITLE = "Slenderman";
const bool DEBUG = true;
const bool ILLUMINATE_SCENE = true;

// COSTANTI PER LA GENERAZIONE DELLA MAPPA
// -------------------------------------------------------------------------------------------
// QUAD_SIDE deve essere sempre un numero pari multiplo del VAO_OBJECTS_SIDE corrispondente
const int TREE_QUAD_SIDE = 72;
const int VAO_OBJECTS_SIDE_TREE = 3;
const float TREE_OFFSET = 40.0f;
const int GRASS_QUAD_SIDE = 640;
const int VAO_OBJECTS_SIDE_GRASS = 10;
const float GRASS_OFFSET = 6.0f;
// Non cambiare il fence offset altrimenti le parti di recinto non combaciano
const float FENCE_OFFSET = 16.8f;
const int NUM_FENCES_FOR_SIDE = 160;
// Deve essere minore o uguale di 8
const int NUMBER_POINTS_OF_INTEREST = 8;
// Deve essere minore di NUMBER_POINTS_OF_INTEREST, minore di 8 e maggiore di 1
const int NUM_PAGES = 6;
// Lasciare 1 altrimenti le prestazioni peggiorano di molto
const int INT_OFFSET_VAO_INDEXES = 1;
// ATTENZIONE DA CAMBIARE NEL CASO SI CAMBINO LE DIMENSIONI DELLA MAPPA
// PER ADESSO NON DISEGNA SOLO IL VAO CORRISPONDENTE A (0, 0)
const std::unordered_set<int> K_SET_TO_EXCLUDE = { 300 };
// Indica l'offset che ha il lampione lungo x e lungo z dal centro del VAO
const float STREETLIGHT_POI_OFFSET = 10.0f;
const float SLENDERMAN_OUT_OF_TREE_OFFSET = 5.0f;

const float MAX_PLAYER_DISTANCE_FRONT = -1340.0f;
const float MAX_PLAYER_DISTANCE_BACK = 1340.0f;
const float MAX_PLAYER_DISTANCE_RIGHT = 1330.0f;
const float MAX_PLAYER_DISTANCE_LEFT = -1350.0f;

// COSTANTI PER LA COLLEZIONE DELLE PAGINE
// -------------------------------------------------------------------------------------------
const float Z_V_MIN_PAGE = -0.35f;
const float X_V_MIN_PAGE = -1.0f;
const float Y_V_MIN_PAGE = -0.2f;

const float Z_V_MAX_PAGE = 0.35f;
const float X_V_MAX_PAGE = 0.0f;
const float Y_V_MAX_PAGE = 0.3f;

const float MAX_ANGLE_PAGE = 0.9f;
const float PAGE_SELECTION_DISTANCE = 10.0f;

const float PAGE_COLLECTED_MESSAGE_SECONDS = 3.0f;

// COSTANTI PER LA GENERAZIONE DELLA MINI MAPPA
// -------------------------------------------------------------------------------------------

const float MAP_DIMENSION = 300.0f;
const float MAP_W_OFFSET = 50.0f;
const float MAP_H_OFFSET = 30.0f;

const float MAP_W_PROP_DIMENSION = MAP_DIMENSION * 2 / (float)SCR_WIDTH;
const float MAP_H_PROP_DIMENSION = MAP_DIMENSION * 2 / (float)SCR_HEIGHT;
const float MAP_W_PROP_OFFSET = MAP_W_OFFSET * 2 / (float)SCR_WIDTH;
const float MAP_H_PROP_OFFSET = MAP_H_OFFSET * 2 / (float)SCR_HEIGHT;

// Deve essere multiplo di 3 e pari aumentandolo aumenta la risoluzione del cerchio
const int NUM_VERTICES_CIRCLE = 60;

const float MAX_W_QUAD_MAP = 1450.0f;

// COLORI
// -------------------------------------------------------------------------------------------
const glm::vec4 RED = glm::vec4(1.0, 0.0, 0.0, 1.0);
const glm::vec4 AABB_COLOR = glm::vec4(0.2, 0.2, 0.2, 1.0);