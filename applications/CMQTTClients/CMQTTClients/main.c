#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <windows.h>
#include "paho.mqtt.c/include/MQTTClient.h"

#define ADDRESS     "tcp://192.168.178.37:1883"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         0
#define TIMEOUT     10000L

/*void test1_sendAndReceive(MQTTClient* c, int qos, char* test_topic)
{
	MQTTClient_deliveryToken dt;
	MQTTClient_message pubmsg = MQTTClient_message_initializer;
	MQTTClient_message* m = NULL;
	char* topicName = NULL;
	int topicLen;
	const int iterations = 50;
	int failures = 0;

	printf("%d messages at QoS %d\n", iterations, qos);
	pubmsg.payload = "a much longer message that we can shorten to the extent that we need to payload up to 11";
	pubmsg.payloadlen = 11;
	pubmsg.qos = qos;
	pubmsg.retained = 0;

	int rc = MQTTClient_subscribe(c, test_topic, qos);

	for (int i = 0; i< iterations; ++i)
	{
		if (i % 10 == 0)
			rc = MQTTClient_publish(c, test_topic, pubmsg.payloadlen, pubmsg.payload, pubmsg.qos, pubmsg.retained, &dt);
		else
			rc = MQTTClient_publishMessage(c, test_topic, &pubmsg, &dt);

		if (qos > 0)
		{
			rc = MQTTClient_waitForCompletion(c, dt, 5000L);
		}

		rc = MQTTClient_receive(c, &topicName, &topicLen, &m, 5000);
		if (topicName)
		{
			printf("Message received on topic %s is %.*s\n", topicName, m->payloadlen, (char*)(m->payload));
			if (pubmsg.payloadlen != m->payloadlen ||
				memcmp(m->payload, pubmsg.payload, m->payloadlen) != 0)
			{
				failures++;
				printf("Error: wrong data - received lengths %d %d\n", pubmsg.payloadlen, m->payloadlen);
				break;
			}
			MQTTClient_free(topicName);
			MQTTClient_freeMessage(&m);
		}
		else
			printf("No message received within timeout period\n");
	}

	MQTTClient_receive(c, &topicName, &topicLen, &m, 2000);
	while (topicName)
	{
		printf("Message received on topic %s is %.*s.\n", topicName, m->payloadlen, (char*)(m->payload));
		MQTTClient_free(topicName);
		MQTTClient_freeMessage(&m);
		MQTTClient_receive(c, &topicName, &topicLen, &m, 2000);
	}
}*/

void publish(MQTTClient* client, int qos, int delay)
{
	while (1)
	{
		MQTTClient_deliveryToken token;
		MQTTClient_message pubmsg = MQTTClient_message_initializer;
		pubmsg.payload = PAYLOAD;
		pubmsg.payloadlen = strlen(PAYLOAD);
		pubmsg.qos = qos;
		pubmsg.retained = 0;

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
		printf("Publish: %s <pub> <clientid> <delay>\n", argv[0]);
		printf("Subscribe: %s <sub> <clientid>\n", argv[0]);
		return -1;
	}

	if (0 == strcmp(argv[1], "pub") && 4 == argc)
	{
		publisher = TRUE;
	}
	else if (0 == strcmp(argv[1], "sub"))
	{
		publisher = FALSE;
	}
	else
	{
		printf("Publish: %s <pub> <clientid> <delay>\n", argv[0]);
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
		publish(client, QOS, atoi(argv[3]));
	}
	else
	{
		subscribe(client, QOS);
	}

	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(client);

	return 0;
}