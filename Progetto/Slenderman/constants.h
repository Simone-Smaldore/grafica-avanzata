#pragma once
// COSTANTI DI CONFIGURAZIONE DEL GIOCO
// -------------------------------------------------------------------------------------------
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;
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
const int NUMBER_POINTS_OF_INTEREST = 10;
// Lasciare 1 altrimenti le prestazioni peggiorano di molto
const int INT_OFFSET_VAO_INDEXES = 1;


