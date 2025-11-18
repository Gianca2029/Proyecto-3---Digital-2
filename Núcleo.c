/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Código adaptado para Proyecto 3 ED2
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs_sd.h"
#include "string.h"
#include "stdio.h"
#include "ili9341.h"
#include "bitmaps.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
SPI_HandleTypeDef hspi1;
FATFS fs;
FIL file;
FRESULT fr;
char sdBuffer[128];
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t msgInicio[] = "\r\nSistema de adquisicion listo\r\n";
uint8_t msgBtn1[]   = "Boton 1: solicitar lectura al ESP32\r\n";
uint8_t msgBtn2[]   = "Boton 2: almacenar valor en SD\r\n";

uint8_t flagBtn1 = 0;
uint8_t flagBtn2 = 0;

uint8_t ultimoValor = 0;
uint8_t tramaNeo[1];

extern uint8_t pista[];
extern uint8_t toad[];
extern uint8_t yoshi[];
extern uint8_t peach[];
extern uint8_t dialogo[];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */
void uartPC(const char *txt);
void dibujarPantallaInicio(void);
uint8_t solicitarDatoESP32(void);
void actualizarNeoPixel(uint8_t codigo);
void guardarDatoEnSD(uint8_t valor);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void uartPC(const char *txt)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)txt, strlen(txt), HAL_MAX_DELAY);
}

void dibujarPantallaInicio(void)
{
	LCD_Init();
	LCD_Clear(0x0000);
	LCD_Bitmap(0, 0,   320, 240, pista);
	LCD_Bitmap(15,  170, 70, 71, yoshi);
	LCD_Bitmap(120, 170, 70, 66, toad);
	LCD_Bitmap(220, 170, 71, 65, peach);
	LCD_Bitmap(90,  70,  70, 83, dialogo);
	LCD_Print("Esperando lectura", 60, 40, 2, 0xFFFF, 0x0000);
}

uint8_t solicitarDatoESP32(void)
{
	uint8_t comando = 'R';
	uint8_t valor = 0;
	HAL_UART_Transmit(&huart3, &comando, 1, HAL_MAX_DELAY);
	if (HAL_UART_Receive(&huart3, &valor, 1, 500) == HAL_OK)
	{
		return valor;
	}
	else
	{
		return 0xFF;
	}
}

void actualizarNeoPixel(uint8_t codigo)
{
	tramaNeo[0] = codigo;
	HAL_UART_Transmit(&huart3, tramaNeo, 1, HAL_MAX_DELAY);
}

void guardarDatoEnSD(uint8_t valor)
{
	char linea[64];
	UINT escritos;

	sprintf(linea, "Valor medido: %u\r\n", valor);

	fr = f_mount(&fs, "", 1);
	if (fr != FR_OK)
	{
		uartPC("No se pudo montar la SD\r\n");
		return;
	}

	fr = f_open(&file, "lecturas.txt", FA_OPEN_APPEND | FA_WRITE);
	if (fr != FR_OK)
	{
		uartPC("No se pudo abrir el archivo\r\n");
		f_mount(NULL, "", 1);
		return;
	}

	f_write(&file, linea, strlen(linea), &escritos);
	f_close(&file);
	uartPC("Dato almacenado en lecturas.txt\r\n");

	f_mount(NULL, "", 1);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  HAL_Init();

  SystemClock_Config();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MX_FATFS_Init();
  MX_USART3_UART_Init();

  /* USER CODE BEGIN 2 */
  uartPC((char*)msgInicio);
  uartPC((char*)msgBtn1);
  uartPC((char*)msgBtn2);

  dibujarPantallaInicio();
  actualizarNeoPixel(1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (flagBtn1 == 1)
	  {
		  uartPC("Solicitando valor al ESP32...\r\n");
		  uint8_t v = solicitarDatoESP32();
		  if (v != 0xFF)
		  {
			  ultimoValor = v;
			  char textoLCD[16];
			  sprintf(textoLCD, "Valor: %u", ultimoValor);
			  LCD_Print("Lectura recibida   ", 40, 40, 2, 0xFFFF, 0x0000);
			  LCD_Print(textoLCD,             80, 100, 3, 0xFFFF, 0x0000);

			  char msgPC[48];
			  sprintf(msgPC, "Lectura: %u\r\n", ultimoValor);
			  uartPC(msgPC);

			  actualizarNeoPixel(2);
		  }
		  else
		  {
			  uartPC("No hubo respuesta del ESP32\r\n");
		  }
		  flagBtn1 = 0;
	  }

	  if (flagBtn2 == 1)
	  {
		  uartPC("Guardando valor en SD...\r\n");
		  guardarDatoEnSD(ultimoValor);
		  actualizarNeoPixel(3);
		  flagBtn2 = 0;
	  }

	  HAL_Delay(50);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  /* Deja aquí la configuración generada por CubeMX */
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{
  /* Deja aquí la configuración generada por CubeMX */
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{
  /* Deja aquí la configuración generada por CubeMX */
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{
  /* Deja aquí la configuración generada por CubeMX */
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* Deja aquí la configuración generada por CubeMX
     asegurando que BTN1_Pin y BTN2_Pin estén como IT_FALLING */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == BTN1_Pin)
	{
		flagBtn1 = 1;
	}
	if (GPIO_Pin == BTN2_Pin)
	{
		flagBtn2 = 1;
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
