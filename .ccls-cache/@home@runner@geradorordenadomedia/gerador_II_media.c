#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "string.h"

typedef struct {
    int hora, min, seg;
} t_time_stamp;

typedef struct {
    int id;
    int sensor_type;
    char medida[10];
    double alpha;
    t_time_stamp timestamp;
} SensorData;

typedef struct node {
    SensorData data;
    struct node *left;
    struct node *right;
} Node;

typedef struct {
    double sum;
    int count;
} HourlyData;

Node* createNode(SensorData data) {
    Node* newNode = malloc(sizeof(Node));
    if (!newNode) {
        printf("Erro na alocação de memória\n");
        exit(0);
    }
    newNode->data = data;
    newNode->left = newNode->right = NULL;
    return newNode;
}

Node* insertNode(Node* root, SensorData data) {
    if (root == NULL)
        return createNode(data);

    // Comparando os atributos para decidir onde inserir o novo nó
    if (data.id < root->data.id)
        root->left = insertNode(root->left, data);
    else if (data.id > root->data.id)
        root->right = insertNode(root->right, data);
    else if (data.sensor_type < root->data.sensor_type)
        root->left = insertNode(root->left, data);
    else if (data.sensor_type > root->data.sensor_type)
        root->right = insertNode(root->right, data);
    else if (data.timestamp.hora < root->data.timestamp.hora)
        root->left = insertNode(root->left, data);
    else if (data.timestamp.hora > root->data.timestamp.hora)
        root->right = insertNode(root->right, data);
    else if (data.timestamp.min < root->data.timestamp.min)
        root->left = insertNode(root->left, data);
    else if (data.timestamp.min > root->data.timestamp.min)
        root->right = insertNode(root->right, data);
    else if (data.timestamp.seg < root->data.timestamp.seg)
        root->left = insertNode(root->left, data);
    else if (data.timestamp.seg > root->data.timestamp.seg)
        root->right = insertNode(root->right, data);
    else if (data.alpha < root->data.alpha)
        root->left = insertNode(root->left, data);
    else if (data.alpha > root->data.alpha)
        root->right = insertNode(root->right, data);

    return root;
}

void inorderTraversal(Node* root) {
    if (root != NULL) {
        inorderTraversal(root->left);
        printf("%d %d %s %02d:%02d:%02d %.2lf\n", root->data.id, root->data.sensor_type, root->data.medida, root->data.timestamp.hora, root->data.timestamp.min, root->data.timestamp.seg, root->data.alpha);
        inorderTraversal(root->right);
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

    int samples, reps=24; 
    int tipos=4;
    double alphas[]={0.0, 0.2, 0.22, 0.25, 0.5}; 

    scanf("%d", &samples);
    int ids[samples+1]; 
    for(int i=0;i<=samples;i++)
        ids[i]=-1;

    Node* root = NULL; // Inicializando a árvore

    HourlyData hourlyData[24] = {0}; // Inicializando a estrutura de dados para a média por hora

    for (int i=1;i<=(samples*reps);i++){

        int id = sensor(samples);

        int t_sensor = (ids[id]==-1?sensor(tipos):ids[id]);
        ids[id] = t_sensor;

        t_time_stamp t = timestamp();
        SensorData new_data;
        new_data.id = id;
        new_data.sensor_type = t_sensor;
        new_data.timestamp = t;
        new_data.alpha = alphas[t_sensor];

        if (t_sensor == 1){ //temperatura
            double temp = temperatura(17, 50);
            sprintf(medida,"%.3lf",temp);
            strcpy(new_data.medida, medida);
        } else if (t_sensor == 2){ // luminosidade
            int lumen = luminosidade(10);
            sprintf(medida,"%d",lumen);
            strcpy(new_data.medida, medida);
        } else if(t_sensor == 3){ // movimento
            int duracao = movimento(1000, 1000*60);
            sprintf(medida,"%d",duracao); 
            strcpy(new_data.medida, medida);
        } else { // umidade
            int umid = umidade(75, 40); 
            sprintf(medida,"%d",umid); 
            strcpy(new_data.medida, medida);
        }

        root = insertNode(root, new_data); // Inserindo os dados na árvore

        // Atualizando os dados da média por hora
        int hour = new_data.timestamp.hora;
        hourlyData[hour].sum += atof(new_data.medida);
        hourlyData[hour].count++;
        }   

        // Calculando a média por hora e imprimindo a saída formatada
        printf("saida com média:\n");
        for (int hour = 0; hour < 24; hour++) {
        if (hourlyData[hour].count > 0) {
            double average = hourlyData[hour].sum / hourlyData[hour].count;
            printf("%d 00:00:00 %.2lf\n", hour, average);
        }
        }

        return 0;
        }

