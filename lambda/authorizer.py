import json


def lambda_handler(event, context):
    return {
        "isAuthenticated": True,
        "principalId": "xxxxxxxx",
        "disconnectAfterInSeconds": 86400,
        "refreshAfterInSeconds": 300,
        "policyDocuments": [
            {
                "Version": "2012-10-17",
                "Statement": [
                    {
                        "Effect": "Allow",
                        "Action": "iot:Connect",
                        "Resource": "*"
                    },
                    {
                        "Effect": "Allow",
                        "Action": "iot:Publish",
                        "Resource": "*"
                    },
                    {
                        "Effect": "Allow",
                        "Action": "iot:Subscribe",
                        "Resource": "*"
                    },
                    {
                        "Effect": "Allow",
                        "Action": "iot:Receive",
                        "Resource": "*"
                    }
                ]
            }
        ]

    }
