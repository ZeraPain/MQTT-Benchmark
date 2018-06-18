#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <windows.h>
#include "mqtt/include/MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define TOPIC       "/home/temperature"
#define QOS         0
#define TIMEOUT     10000L

void publish(MQTTClient* client, int qos, int payloadLen, int delay)
{
	MQTTClient_deliveryToken token;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;

	void* mem = malloc(payloadLen);
	pubmsg.payload = mem;
	pubmsg.payloadlen = payloadLen;
	pubmsg.qos = qos;
	pubmsg.retained = 0;
	while (1)
	{
		int rc = MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
		if (rc < 0)
		{
			printf("MQTTClient_publishMessage failed\n");
		}

		if (qos > 0)
		{
			rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
			if (rc < 0)
			{
				printf("MQTTClient_waitForCompletion failed\n");
			}
		}


		Sleep(delay);
	}
	free(mem);
}

void subscribe(MQTTClient* client, int qos)
{
	MQTTClient_message* m = NULL;
	char* topicName = NULL;
	int topicLen;

	int rc = MQTTClient_subscribe(client, TOPIC, qos);
	if (MQTTCLIENT_SUCCESS != rc)
	{
		printf("MQTTClient_subscribe failed\n");
	}

	while (1)
	{
		rc = MQTTClient_receive(client, &topicName, &topicLen, &m, 5000);
		if (MQTTCLIENT_SUCCESS != rc)
		{
			printf("MQTTClient_receive failed\n");
		}

		if (topicName)
		{
			//printf("Message received on topic %s is %.*s\n", topicName, m->payloadlen, (char*)(m->payload));
			MQTTClient_free(topicName);
			MQTTClient_freeMessage(&m);
		}
		else
		{
			printf("No message received within timeout period\n");
		}
	}
}

int main(int argc, char* argv[])
{
	BOOL publisher = FALSE;

	if (argc < 3)
	{
		printf("Publish: %s <pub> <clientid> <payloadLen> <delay>\n", argv[0]);
		printf("Subscribe: %s <sub> <clientid>\n", argv[0]);
		return -1;
	}

	if (0 == strcmp(argv[1], "pub") && 5 == argc)
	{
		publisher = TRUE;
	}
	else if (0 == strcmp(argv[1], "sub"))
	{
		publisher = FALSE;
	}
	else
	{
		printf("Publish: %s <pub> <clientid> <payloadLen> <delay>\n", argv[0]);
		printf("Subscribe: %s <sub> <clientid>\n", argv[0]);
		return -1;
	}

	MQTTClient client;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	int rc;

	MQTTClient_create(&client, ADDRESS, argv[2], MQTTCLIENT_PERSISTENCE_NONE, NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;

	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("Failed to connect, return code %d\n", rc);
		exit(-1);
	}

	if (publisher)
	{
		printf(argv[4]);
		publish(client, QOS, atoi(argv[3]), atoi(argv[4]));
	}
	else
	{
		subscribe(client, QOS);
	}

	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(client);

	return 0;
}