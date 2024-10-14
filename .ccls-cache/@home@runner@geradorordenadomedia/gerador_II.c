#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"


typedef struct {
    int hora, min, seg;
} t_time_stamp;

typedef struct no t_no;
struct no{
    void* info;
    t_no *sae;
    t_no *sad;
    t_no *ancestral;
};

t_no* criar_no(void* info, t_no* ancestral){
    t_no* novo = malloc(sizeof(t_no));

    novo->info = info;
    novo->ancestral = ancestral;
    novo->sad = NULL;
    novo->sae = NULL;

    return novo;
}

struct abb{
    t_no* raiz;
    int tamanho;

    t_imprimir_abb impressora;
    t_comparar_abb comparar;
};

t_abb* criar_abb(t_imprimir_abb impressora, t_comparar_abb comparar){
    t_abb* nova = malloc(sizeof(t_abb));
    nova->raiz = NULL;
    nova->tamanho=0;
    nova->impressora = impressora;
    nova->comparar = comparar;

    return nova;
}

static t_no* _inserir_abb(t_no* raiz, t_no* ancestral, void* info, t_comparar_abb comparar){
    if (raiz == NULL){
        return criar_no(info, ancestral);
    }else{
        int k = comparar(raiz->info, info);
        if (k >= 0){ // inserir esquerda

            raiz->sae = _inserir_abb(raiz->sae,raiz, info, comparar);
        }else{

            raiz->sad = _inserir_abb(raiz->sad, raiz, info, comparar);
        }
        return raiz;
    }
}

void inserir_abb(t_abb *abb, void* info){

    abb->raiz = _inserir_abb(abb->raiz, NULL, info, abb->comparar);
    abb->tamanho++;

}

t_no* _buscar_abb(t_no* raiz, void* chave, t_comparar_abb comparar){

    if (raiz == NULL){
        return NULL;
    }else{
        int status = comparar(raiz->info, chave);
        if (status == 0){
            return raiz;
        }else if(status > 0){
            return _buscar_abb(raiz->sae, chave, comparar);
        }else{
            return _buscar_abb(raiz->sad, chave, comparar);
        }
    }

}

t_time_stamp timestamp() {
  time_t current_time = time(NULL);
  
  struct tm *local_time = localtime(&current_time);

  int hour = (local_time->tm_hour + rand()) % 24;
  int minute = (local_time->tm_min + rand()) % 60;
  int second = (local_time->tm_sec + rand()) % 60;

  return (t_time_stamp){.hora=hour, .min = minute, .seg =second};
}


int sensor(int nro_sensores){
    static int rep = 0;
    static int passes = 0;
    if ( (rep == 0) && (passes <= 0) ){
        int vezes = rand() % 100;
        for( int i=1;i<vezes;i++)
            rand();
        rep = 1;
    }else{
        passes = rand() % 4;
        rep = 0;
    }
    passes--;
    return (1 + (rand()%nro_sensores));
}

#define MIN_TEMP -10 // Minimum measurable temperature (°C)
#define MAX_TEMP 50 // Maximum measurable temperature (°C)

double temperatura(float baseTemp, float ambientTemp) {
  float temperatureChange = 0.1 * (ambientTemp - baseTemp); 
  float noise = (float)rand() / RAND_MAX * 0.5; 
  temperatureChange += noise;
  float newTemp = baseTemp + temperatureChange;

  newTemp = (newTemp < MIN_TEMP) ? MIN_TEMP : newTemp;
  newTemp = (newTemp > MAX_TEMP) ? MAX_TEMP : newTemp;

  return newTemp;
}

#define SENSOR_RANGE 5 // Detection range in meters (adjust as needed)

int movimento(int short_duration, int long_duration){

    int movement = rand() % 10; 
    if (movement < 3){ 
        return rand()%long_duration;
    }else{
        return rand()%short_duration;
    }
}

#define MIN_LUMEN 0 // Minimum measurable lumen value
#define MAX_LUMEN 1000 // Maximum measurable lumen value

int luminosidade(int ambientLight) {
  int baseValue = ambientLight * (rand() % 50); 
  int noise = rand() % 10; 
  baseValue += noise;

  baseValue = (baseValue < MIN_LUMEN) ? MIN_LUMEN : baseValue;
  baseValue = (baseValue > MAX_LUMEN) ? MAX_LUMEN : baseValue;

  return baseValue;
}


#define MIN_HUMIDITY 0 // Minimum measurable humidity percentage (%)
#define MAX_HUMIDITY 100 // Maximum measurable humidity percentage (%)

int umidade(int baseHumidity, int ambientTemp) {
  int humidityChange = (ambientTemp - 20) * 2; 
  int noise = rand() % 5; 
  int newHumidity = baseHumidity + humidityChange + noise;
  
  newHumidity = (newHumidity < MIN_HUMIDITY) ? MIN_HUMIDITY : newHumidity;
  newHumidity = (newHumidity > MAX_HUMIDITY) ? MAX_HUMIDITY : newHumidity;

  return newHumidity;
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    char medida[10];

    int samples, reps=20; 
    int tipos=4;
    double alphas[]={0.0, 0.2, 0.22, 0.25, 0.5}; 

    scanf("%d", &samples);
    int ids[samples+1]; 
    for(int i=0;i<=samples;i++)
        ids[i]=-1;

    for (int i=1;i<=(samples*reps);i++){

        int id = sensor(samples);
        int t_sensor = (ids[id]==-1?sensor(tipos):ids[id]);
        ids[id] = t_sensor;

        t_time_stamp t = timestamp();
        if (t_sensor == 1){ //temperatura
            double temp = temperatura(17, 50);
            sprintf(medida,"%.3lf",temp);

        }else if (t_sensor == 2){ // luminosidade
            int lumen = luminosidade(10);
            sprintf(medida,"%d",lumen);

        }else  if(t_sensor == 3){ // movimento
            int duracao = movimento(1000, 1000*60);
            sprintf(medida,"%d",duracao);

        }else { // umidade
            int umid = umidade(75, 40); 
            sprintf(medida,"%d",umid);
        }
        printf("%d %d %s %02d:%02d:%02d %.2lf\n", id, t_sensor, medida, t.hora, t.min, t.seg, alphas[t_sensor] ); 

    }   
  
    return 0;
}