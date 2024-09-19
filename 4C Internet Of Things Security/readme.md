# Project 4C: Secure Embedded Sensor Communication Interface

## Introduction

The Internet of Things (IoT) connects a growing array of sensors and appliances, increasingly exposing these devices to remote attacks as they link to the Internet. This project addresses the importance of securing such devices by implementing both unencrypted and encrypted communication channels for an embedded temperature sensor application.

## Project Description

### Part 1: Communication with a Logging Server

The goal was to extend an embedded temperature sensor to interact with a network server. This involved:

- Developing the `lab4c_tcp` application to:
  - Connect to a server over TCP.
  - Send temperature data and device ID.
  - Handle and log commands from the server.
  - Ensure proper error handling and logging of communication issues.

### Part 2: Authenticated TLS Session Encryption

The project further involved securing communication with TLS by developing the `lab4c_tls` application to:
  - Establish a secure TLS connection to the server.
  - Authenticate the server with a public key certificate.
  - Send temperature data and device ID over the encrypted channel.
  - Handle and log commands, ensuring the secure transmission of data.

## My Contributions

- **Design and Implementation**: Developed both `lab4c_tcp` and `lab4c_tls` applications, ensuring they met project requirements for both unencrypted and encrypted communication.
- **Secure Communication**: Implemented TLS encryption to safeguard data transmission, demonstrating proficiency in using cryptographic libraries and secure socket layer encryption.
- **Testing and Debugging**: Conducted thorough testing of both applications, debugging issues related to network communication and encryption, and ensuring compatibility with the Beaglebone hardware.

## Significance

This project underscores the critical need for security in IoT devices, demonstrating practical skills in developing secure communication interfaces. It reflects an understanding of both fundamental and advanced principles of cryptography and secure network communication, applicable to a wide range of embedded systems and IoT applications.

## Conclusion

Project 4C highlights the importance of securing IoT devices against potential attacks by implementing both unencrypted and encrypted communication channels. By developing and testing robust applications for secure data transmission, this project contributes to the ongoing efforts to enhance IoT security and safeguard sensitive information in connected environments.

---

Feel free to explore the provided code and documentation to understand the implementation details and testing processes. For any inquiries or further discussions, please reach out via the contact details in the README file.
