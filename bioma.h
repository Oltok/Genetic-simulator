#pragma once //ni idea que hace, decian que es para evitar que el archivo se incluya multiples veces
#include <vector>
#include <raymath.h>

// he pasado las constantes aquí 
const float terrainWidth = 2000.0f; //2000, 2000, 1000, 100, 100
const float terrainDepth = 2000.0f;
const float terrainMaxHeight = 1000.0f;
const int terrainImgWidth = 100;
const int terrainImgHeight = 100;
const int terrainBarrier = 10;


enum class BaseBiome {
    FIELD,
    JUNGLE,
    DESERT,
    ICE,
    TUNDRA,
    ACQUATIC 
};

// guardar información de cada coordenada
struct CoordsData {
    float height;
    float temperature;
    float humidity;
    float quality;
    float quantity;
    BaseBiome biome;
};


class World {
public:
    static std::vector<CoordsData> mapData;
    static int mapWidth;
    static int mapHeight;
    static float worldWidth;
    static float worldDepth;

    static void GenerateBiomes() {
        for (int i = 0; i < mapWidth * mapHeight; i++) {
            CoordsData& coord = mapData[i];

            // logica de biomas
            if (coord.height < 0.30f) {
                coord.biome = BaseBiome::ACQUATIC;
                continue;
            }

            bool isTemperate = (coord.temperature >= 0.4f && coord.temperature <= 0.6f);
            bool isModerate = (coord.humidity >= 0.4f && coord.humidity <= 0.6f);

            bool isHot = coord.temperature > 0.5f;
            bool isWet = coord.humidity > 0.5f;

            if (isTemperate && isModerate) coord.biome = BaseBiome::FIELD;
            else if (!isHot && !isWet) coord.biome = BaseBiome::ICE;
            else if (!isHot && isWet)  coord.biome = BaseBiome::TUNDRA;
            else if (isHot && !isWet)  coord.biome = BaseBiome::DESERT;
            else if (isHot && isWet)   coord.biome = BaseBiome::JUNGLE;
            else coord.biome = BaseBiome::FIELD; //por si aca

        }
    }

    static void SmoothBiomes(int iterations) {

        for (int i = 0; i < iterations; i++) {
            
            std::vector<CoordsData> tempMap = mapData; // copia temporal del mapa para hacer el smoothing

            for (int y = 0; y < mapHeight; y++) {
                for (int x = 0; x < mapWidth; x++) {

                    int currentIndex = y * mapWidth + x;

                    if (mapData[currentIndex].height < 0.20f) continue; // el thr de agua no tiene smoothing
                    int biomeCounts[6] = {0};

                    //revisar los pixeles vecinos
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dx = -1; dx <= 1; dx++) {
                            int nx = x + dx;
                            int ny = y + dy;
                            
                            // para no salirnos del mapa
                            if (nx >= 0 && nx < mapWidth && ny >= 0 && ny < mapHeight) {
                                int neighborIndex = ny * mapWidth + nx;
                                BaseBiome b = mapData[neighborIndex].biome;
                                biomeCounts[(int)b]++;
                            }
                        }
                    }

                    // hacer el average para seleccionar el biona
                    int votos = 0;
                    BaseBiome majorityBiome = mapData[currentIndex].biome;

                    for (int b = 0; b < 6; b++) {
                        if (biomeCounts[b] > votos && (BaseBiome)b != BaseBiome::ACQUATIC) {
                            votos = biomeCounts[b];
                            majorityBiome = (BaseBiome)b;
                        }
                    }
                    
                    tempMap[currentIndex].biome = majorityBiome;
                }
            }

            mapData = tempMap;
        }
    }
    static BaseBiome GetBaseBiome(float x, float z) {
        
        // coordenada del mundo a coordenada de la grid
        int gridX = (int)((x / worldWidth) * mapWidth);
        int gridZ = (int)((z / worldDepth) * mapHeight);

        // para no salirnos de los limites
        if (gridX < 0) gridX = 0;
        if (gridX >= mapWidth) gridX = mapWidth -1;
        if (gridZ < 0) gridZ = 0;
        if (gridZ >= mapHeight) gridZ = mapHeight -1;

        int index = gridZ * mapWidth + gridX;

        return mapData[index].biome;
    }
};

std::vector<CoordsData> World::mapData;
int World::mapWidth;
int World::mapHeight;
float World::worldWidth;
float World::worldDepth;