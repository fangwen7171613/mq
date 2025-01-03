package mq;

/**
 * This class encapsulates the functionality to interact with a message queue (MQ) through native methods.
 * It demonstrates how to publish and handle messages with MQ using JNI (Java Native Interface).
 */
public class MqClass {
    // Native methods declarations to interact with the message queue
    public native int init(String workRoot, int clientId, int[] topicIds);
    public native void exit();
    public native int publishTopicPollMessage(int topicId, String message, int msgSize);
    public native int publishTopicBroadcastMessage(int topicId, String message, int msgSize);
    public native int publishAddressedMessage(int clientId, int nId, int topicId, String message, int msgSize);

    // Class-level variables to track message count and MQ status
    private static int messageCount = 0;
    private static volatile boolean isMqExited = false;

    // Callback method implementations for MQ status updates
    public static void onReady() {
        System.out.println("Java: MQ is ready for operation.");
    }

    public static void onNotReady() {
        System.out.println("Java: MQ is currently not ready.");
    }

    public static void onExit() {
        System.out.println("Java: MQ service is exiting.");
        isMqExited = true;
    }

    // Callback for handling addressed messages received from MQ
    public static void onPublishAddressedMessage(int clientId, int nId, int topicId, String message, int msgSize) {
        // System.out.printf("Received message: %s [Size: %d]%n", message, msgSize);
        messageCount++;
        topicId = 6; // Example topic ID
        message = "123"; // Example message content

        new MqClass().publishAddressedMessage(clientId, nId, topicId, message, message.length());
    }

    // Publishes a predefined message to a specific topic
    private static void publishMessage() {
        final int topicId = 6; // Example topic ID
        final String message = "123"; // Example message content

        new MqClass().publishTopicPollMessage(topicId, message, message.length());
    }

    // Static block to load the native library on startup
    static {
        System.loadLibrary("jmq_sdk");
    }

    /**
     * Main method to initiate the MQ test.
     * @param args Command-line arguments (not used).
     */
    public static void main(String[] args) {
        MqClass mq = new MqClass();
        String workRoot = System.getenv("WORK_ROOT");

        if (workRoot == null || workRoot.isEmpty()) {
            System.err.println("Error: WORK_ROOT environment variable is not set or is empty.");
            return; // Terminate execution if WORK_ROOT is not configured
        }

        // Initialize MQ with the given work root and topic IDs
        int[] topicIds = {5, 55}; // Example topic IDs, adjust as needed
        mq.init(workRoot, 2, topicIds);

        // Main loop to keep the application running until MQ exits
        while (!isMqExited) {
            try {
                Thread.sleep(1000);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt(); // Handle interrupted exception
                System.err.println("Thread interrupted: " + e.getMessage());
            }
        }

        mq.exit();
        System.out.println("MQ test has completed and exited successfully.");
    }
}
