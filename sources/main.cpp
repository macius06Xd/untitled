#include <dim/dimension3D.hpp>
#include <set>
#include <functional>
#include <queue>
#include <chrono>
#include <thread>
#include <fstream>
#include <mutex>
#include <cstring>
#include <sstream>
#pragma warning(disable : 4996)
#include <winsock2.h>
#include <ws2tcpip.h>
constexpr int port = 8020;
namespace std {
	template<>
	struct hash<dim::Vector3>
	{
		
		size_t operator()(const dim::Vector3& v) const
		{
			return ((size_t)v.x ^ (size_t)v.y) ^ (size_t)v.z;
		}
	};

	template<>
	struct equal_to<dim::Vector3>
	{
		bool operator()(const dim::Vector3& v1, const dim::Vector3& v2) const
		{
			return (v1.x == v2.x) && (v1.y == v2.y) && (v1.z == v2.z);
		}
	};

		
}
std::vector<dim::Vector3> generate_pyramid_vertices ( int size, float square_size)
{
	dim::Vector3 start = dim::Vector3(0, 0, 0);
	std::vector<dim::Vector3> Points;
	int level_size = size;
	dim::Vector3 point = start;
	while (level_size - 1)
	{
		auto starting_point = point;
		for (int i = 0; i < level_size - 1; i++)
		{
			point = point + dim::Vector3{ 0,square_size,0 };
			Points.push_back(point);
		}
		for (int i = 0; i < level_size - 1; i++)
		{
			point = point + dim::Vector3{ square_size,0,0 };
			Points.push_back(point);
		}
		for (int i = 0; i < level_size - 1; i++)
		{
			point = point + dim::Vector3{ 0,-square_size,0 };
			Points.push_back(point);
		}
		for (int i = 0; i < level_size - 1; i++)
		{
			point = point + dim::Vector3{ -square_size,0,0 };
			Points.push_back(point);
		}
		point = starting_point + dim::Vector3(square_size / 2, square_size / 2 , square_size);
		level_size--;
	}
	Points.push_back(point);
	return Points;


}

std::string vectortotext(std::vector<dim::Vector3> wektors)
{
	std::string returnvalue;
	for (auto wektor : wektors)
	{
		returnvalue += "" + std::to_string(wektor.x) + " " + std::to_string(wektor.y) + " " + std::to_string(wektor.z) + "\n";
	}
	return returnvalue;
}

bool collides(dim::Object* robot, dim::Vector3 newPosition, dim::Object* object)
{
	const float buffer = -0.1f; // Buffer zone around each object
	dim::Vector3 robotSize = robot->get_size();
	dim::Vector3 objectSize = object->get_size() + dim::Vector3(buffer, buffer, buffer); // Expand the object's size
	dim::Vector3 robotPositionCenter = newPosition;
	dim::Vector3 objectPositionCenter = object->get_position();
	// Calculate the minimum and maximum extents of both the cubes along each axis
	dim::Vector3 robotMin = robotPositionCenter - (robotSize / 2.0f);
	dim::Vector3 robotMax = robotPositionCenter + (robotSize / 2.0f);
	dim::Vector3 objectMin = objectPositionCenter - (objectSize / 2.0f);
	dim::Vector3 objectMax = objectPositionCenter + (objectSize / 2.0f);

	// Check if the extents overlap along each axis
	bool xOverlap = robotMin.x <= objectMax.x && robotMax.x >= objectMin.x;
	bool yOverlap = robotMin.y <= objectMax.y && robotMax.y >= objectMin.y;
	bool zOverlap = robotMin.z <= objectMax.z && robotMax.z >= objectMin.z;

	// If the extents overlap along all axes, the cubes intersect
	return xOverlap && yOverlap && zOverlap;
}
std::vector<dim::Vector3> possibleMoves(dim::Object *robot,dim::Vector3 position, std::vector<dim::Object*> gameObjects)
{

	std::vector<dim::Vector3> possibleMoves = {
		dim::Vector3(1, 0, 0),
		dim::Vector3(-1, 0, 0),
		dim::Vector3(0, 1, 0),
		dim::Vector3(0, -1, 0)
	};

	for (auto move : possibleMoves)
	{
		dim::Vector3 newRobotPosition = position + move;

		bool collidesWithObject = false;
		for (auto object : gameObjects)
		{
			if (object != robot)
			{
				if (collides(robot, newRobotPosition, object))
				{
					//collidesWithObject = true;
					break;
				}
			}
		}

		if (!collidesWithObject)
		{
			possibleMoves.push_back(dim::Vector3(0, 0, 1)); // Add new move
			break;
		}
	}
	return possibleMoves;
}
bool is_collision(dim::Vector3 pos, dim::Object* robot, std::vector<dim::Object*> objects, dim::Object * block , dim::Object * partner) {


// Check for collision with other objects
for (dim::Object* obj : objects) {
	if (obj != robot && obj != block &&  obj !=partner)
	{
		if (collides(robot, pos, obj)) {
			return true;
		}
		if(block != NULL)
		if (collides(block, pos + block->get_position() - robot->get_position(), obj)) {
			return true;
		}
		if(partner!=NULL)
		if (collides(partner, pos+partner->get_position() - robot->get_position(), obj)) {
			return true;
		}
	}
}

// No collision
return false;
}


std::mutex block_adding;
#include <queue>
#include <unordered_map>
#include <vector>
#include <functional>
#include <cmath>

const int robots = 2;
int stone_count = 0;
struct Node {
	dim::Vector3 pos;
	double g, h;
	Node* parent;

	Node(dim::Vector3 pos, double g, double h, Node* parent = nullptr) : pos(pos), g(g), h(h), parent(parent) {}

	double f() const {
		return g + h;
	}
};

struct NodeHash {
	std::size_t operator()(const Node* node) const {
		return std::hash<int>()(std::round(node->pos.x)) ^
			std::hash<int>()(std::round(node->pos.y)) ^
			std::hash<int>()(std::round(node->pos.z));
	}
};

struct NodeEqual {
	bool operator()(const Node* lhs, const Node* rhs) const {
		return lhs->pos == rhs->pos;
	}
};





std::vector<dim::Vector3> findpath(dim::Vector3 start, dim::Vector3 goal, dim::Object* robot, std::vector<dim::Object*> objects, dim::Object* block , dim::Object* partner) {
	// Define a lambda function to calculate the heuristic distance between two points
	auto heuristic = [](dim::Vector3 a, dim::Vector3 b) {
		return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
	};

	// Initialize the open and closed sets
	std::priority_queue<Node*, std::vector<Node*>, std::function<bool(Node*, Node*)>> open(
		[](Node* a, Node* b) {
			return a->f() > b->f();
		});
	std::unordered_map<Node*, Node*, NodeHash, NodeEqual> closed;

	// Create the start node and add it to the open set
	Node* start_node = new Node(start, 0, heuristic(start, goal));
	Node* closest_node = new Node(start, 0, heuristic(start, goal));
	open.push(start_node);

	while (!open.empty()) {
		// Get the node with the lowest f score from the open set
		Node* current_node = open.top();
		open.pop();

		// Check if we have reached the goal
		if (current_node->pos == goal) {
			// Build the path by following the parent pointers
			std::vector<dim::Vector3> path;
			while (current_node) {
				path.push_back(current_node->pos);
				current_node = current_node->parent;
			}
			std::reverse(path.begin(), path.end());



			return path;
		}
		if (!closest_node || heuristic(current_node->pos, goal) < heuristic(closest_node->pos, goal)) {
			closest_node = current_node;
		}

		// Generate the successors of the current node
		const double move_dist = 1.0;
		std::vector<dim::Vector3> moves = {
			dim::Vector3(move_dist, 0, 0),
			dim::Vector3(-move_dist, 0, 0),
			dim::Vector3(0, move_dist, 0),
			dim::Vector3(0, -move_dist, 0),
		};
		std::vector<dim::Vector3> Amoves;

		auto was_Collision = false;
		for (auto& move : moves)
		{
			if (is_collision(current_node->pos + move, robot, objects,block,partner))
			{
				was_Collision = true;
			}
			else if (is_collision(current_node->pos + move + dim::Vector3(0,0,-1), robot, objects,block,partner) || current_node->pos.z < 0.1)
			{
				Amoves.push_back(move);
				}
			}

		
		for (auto& move : Amoves) {
			dim::Vector3 next_pos = current_node->pos + move;

			// Check if the next position is valid (i.e., within bounds and not colliding with any object)
			if (next_pos.x >= -40 && next_pos.x <= 40 &&
				next_pos.y >= -40 && next_pos.y <= 40 &&
				next_pos.z >= 0 && next_pos.z <= 40 )
				 {

				// Calculate the cost of moving to the next position
				double cost = current_node->g + heuristic(current_node->pos, next_pos);

				// Check if we have already visited the next position
				auto next_node_it = closed.find(new Node(next_pos, 0, 0));
				if (next_node_it != closed.end()) {
					Node* next_node = next_node_it->first;
					if (cost < next_node->g) {
						// Update the next node's parent and g score
						next_node->parent = current_node;
						next_node->g = cost;
					}
				}
				else {
					// Create a new node for the next position and add it to the open set
					Node* next_node = new Node(next_pos, cost, heuristic(next_pos, goal), current_node);
					open.push(next_node);
					closed[next_node] = next_node;
				}
			}
		}

		// Add the current node to the closed set
		closed[current_node] = current_node;
	}
	// Return an empty path if no path was found
	return std::vector<dim::Vector3>();
}

std::vector<dim::Object*> pyramid_objects;
int pyramid_count = 0;

auto  pyramid_points = generate_pyramid_vertices( 5, 4);
auto top = pyramid_points.back();
bool myArray[100] = { false };
std::mutex point_searching;
std::mutex position_mutex;
	dim::Vector3 closestpoint(dim::Vector3 object) {
		// Get the center position of the block
		dim::Vector3 block_pos = object;

		// Calculate the vector from the middle position of the pyramid to the block's position
		dim::Vector3 diff = block_pos - top;
		dim::Vector3 vektor;
		// Determine the cardinal direction of the block based on the vector's x and y components
		double x = diff.x;
		double y = diff.y;
		if (std::abs(x) > std::abs(y)) {
			if (x > 0) {
				vektor= dim::Vector3(1, 0, 0); // block is to the east
			}
			else {
				vektor= dim::Vector3(-1, 0, 0); // block is to the west
			}
		}
		else {
			if (y > 0) {
				vektor= dim::Vector3(0, 1, 0); // block is to the north
			}
			else {
				vektor= dim::Vector3(0, -1, 0); // block is to the south
			}
		}
		return vektor;
	}

	class Robot : public dim::Object
	{
	public:
		SOCKET socket;
		SOCKET othersocket;
		char buffer[2048];
		Robot * partner;
		bool captain;
		dim::Object * block;
		bool hold_block = false;
		Robot(dim::Mesh mesh , dim::Material material,SOCKET socket,SOCKET socket2) : dim::Object(mesh,material)
		{
			this->socket = socket;
			this->othersocket = socket2;
		}
		void path_sending(bool both)
		{
			stone_count = pyramid_count;
			block = pyramid_objects[2 + pyramid_count];
			partner->block = block;
			dim::Vector3 point = closestpoint(pyramid_points[pyramid_count]);
			std::swap(point.x, point.y);
			auto target_position = block->get_position() + point * 4;
			auto partner_target_position = block->get_position() + point * -4;
			std::vector<dim::Vector3> path;
			if (!hold_block)
			{
				if (captain)
				path = findpath(this->get_position(), target_position, this, pyramid_objects, NULL, NULL);
				else
				{
					path = findpath(this->get_position(), partner_target_position, this, pyramid_objects, NULL, NULL);
				}
			}
			else
			{
				dim::Vector3 point = closestpoint(pyramid_points[stone_count]);
				target_position = pyramid_points[stone_count] + (point * 8) + this->get_position() - block->get_position();
				path = findpath(this->get_position(), target_position, this, pyramid_objects, block, partner);
			}
			std::string pathbuffer;
			pathbuffer.reserve(10000);
			pathbuffer += "path";

			for (dim::Vector3 vektor : path)
			{
				int x = vektor.x;
				int y = vektor.y;
				// Add x and y coordinates to the buffer
				pathbuffer += std::to_string(x) + "," + std::to_string(y) + ";";
			}

			// Send the pathbuffer to the other side
			send(this->socket, pathbuffer.c_str(), pathbuffer.length(), 0);
			if (hold_block)
			{
				std::vector<dim::Vector3> path2;
				auto diff = this->partner->get_position() - this->get_position();
				for (auto p : path)
				{
					path2.push_back(p + diff);
				}
				pathbuffer.clear();
				pathbuffer.reserve(10000);
				pathbuffer += "path";

				for (dim::Vector3 vektor : path2)
				{
					int x = vektor.x;
					int y = vektor.y;
					// Add x and y coordinates to the buffer
					pathbuffer += std::to_string(x) + "," + std::to_string(y) + ";";
				}

				// Send the pathbuffer to the other side
				send(this->othersocket, pathbuffer.c_str(), pathbuffer.length(), 0);
			}
		}
			

		void ask_move(char buf [1048])
		{
			char answer[10];
			send(othersocket, buf, 1048, 0);
		}

		void action()
		{
			const char* value = "siema\0";
			send(socket, value, strlen(value), 0);
			std::ofstream myfile;
			myfile.open("example.txt", std::ios::app);
			send_position();
			while (1)
			{
				char recive_buffer[1048];
				int count = recv(this->socket, recive_buffer, 1048, 0);
				recive_buffer[count] = '\0';
			
				if (strcmp(recive_buffer, "paths\0") == 0)
				{
					path_sending(true);
				}
				else if (strcmp(recive_buffer, "path\0") == 0)
				{
					path_sending(false);
				}
				else if (strcmp(recive_buffer, "ask_move\0") == 0)
				{
					ask_move(recive_buffer);
				}
				else if (strncmp(recive_buffer, "move", std::strlen("move")) == 0)
				{
					int x, y;
					x = y = 0;
					sscanf_s(recive_buffer, "move {%d,%d}", &x, &y);
					

					if (!is_collision(dim::Vector3(x, y, 0), this, pyramid_objects, NULL, NULL))
					{

						this->set_position(x, y, 0);
						if (hold_block)
						{
							auto pos = this->get_position() + partner->get_position();
							auto pos2 = pos / 2;
							block->set_position(pos2);
						}
						send(socket, "y\0", 3, 0);
					}
					else
					{
						send(socket, "n\0", 3, 0);
					}
				}
				else  if (strcmp(recive_buffer, "hold\0") == 0)
				{
					if (hold_block == TRUE)
					{
						partner->hold_block = false;
						send(othersocket, "drop\0", 6, 0);
					}
					hold_block = !hold_block;
				}
				else  if (strcmp(recive_buffer, "push\0") == 0)
				{

					std::this_thread::sleep_for(std::chrono::seconds(1));
					stone_count++;
					pyramid_count++;
					pyramid_objects[2 + stone_count]->set_position(15, -20, 0);
					block->set_position(pyramid_points[stone_count - 1]);
					send(socket, "push_end\0", 10, 0);
					send(othersocket, "push_end\0", 10, 0);
				}
				else  if (strcmp(recive_buffer, "partner_hold\0") == 0)
				{
					if (!partner->hold_block)
					{
						send(socket, "NO\0", 3, 0);
					}
					else
					{
						send(socket, "yes\0", 3, 0);
					}
				}
				else
				{

					myfile << recive_buffer << std::endl;
					send(othersocket, recive_buffer, 10, 0);
				}
			}
		
		}
		void send_position()
		{
			
			int a, b;
			a = this->get_position().x;
			b = this->get_position().y;
			if (a == 2) captain = true;
			else captain = false;
			std::string message = "n{" + std::to_string(a) + " , " + std::to_string(b) + "}";
			send(socket, message.c_str(), message.length(), 0);
			recv(socket, buffer, 2048, 0);
		}
		void set_partner(Robot* partner)
		{
			this->partner = partner;
		}
	};
int main()
{
	// Open the main window and initialize the libraries
	dim::Window::open("App", 0.75f);

	// Create the scenes
	dim::Scene scene("Scene");

	// Cameras and controllers
	dim::PerspectiveCamera cam;
	cam.set_position(dim::Vector3(50.f, -30.f, 0.f));
	scene.set_camera(cam);

	scene.set_controller(dim::OrbitController());

	// Add lights
	scene.add_light(dim::DirectionalLight(dim::Vector3(-1.f, -1.f, -1.f)));
	scene.add_light(dim::DirectionalLight(dim::Vector3(1.f, 1.f, 1.f)));
	scene.add_light(dim::AmbientLight());

	// Create objects


	
	 //sockets
 WSADATA wsaData;
 WSAStartup(MAKEWORD(2, 2), &wsaData);
 SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
 SOCKADDR_IN serverAddr{};
 serverAddr.sin_family = AF_INET;
 serverAddr.sin_port = htons(port);
 serverAddr.sin_addr.s_addr = INADDR_ANY;
 bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
 SOCKET sockets[2];
 for (int i = 0; i < 2; i++)
 {
	 listen(serverSocket, 1);
	 sockets[i] = accept(serverSocket, NULL, NULL);
 }
	// The example button color
	float color[4] = { 1.f, 1.f, 1.f, 1.f };
	double olddelta = 0;
	double delta = 0;
	double begin_time = clock();
	Robot * robot = new Robot(dim::Mesh::Cube(), dim::Material(dim::Color(0, 1, 1), 0.1f, 0.5f, 0.6f, 30.f),sockets[0],sockets[1]);
	robot->set_size(dim::Vector3(1, 1, 4));
	Robot * robot2 = new Robot(dim::Mesh::Cube(), dim::Material(dim::Color(0, 1, 1), 0.1f, 0.5f, 0.6f, 30.f),sockets[1],sockets[0]);
	robot->set_partner(robot2);
	robot2->set_partner(robot);
	robot2->set_size(dim::Vector3(1, 1, 4));
	pyramid_objects.push_back(robot);
	pyramid_objects.back()->set_position(dim::Vector3(2, 3, 0));
	pyramid_objects.push_back(robot2);
	pyramid_objects.back()->set_position(dim::Vector3(-10, -15, 0));
	int d = 0;
	while (d < pyramid_points.size())
	{

		dim::Object* object_1 = new dim::Object(dim::Mesh::Cube(), dim::Material(dim::Color(0, 0, 1), 0.1f, 0.5f, 0.6f, 30.f));
		object_1->set_size(dim::Vector3(4, 4, 4));
		pyramid_objects.push_back(object_1);
		pyramid_objects.back()->set_position(dim::Vector3(100, 100, 100));
		d++;
	}
	pyramid_objects[2]->set_position(15, -10, 0);
	int p = pyramid_objects.size();
	int k = pyramid_objects.size();// Main loop
	double delta_acum = 0;


	auto start_time = std::chrono::high_resolution_clock::now();

	// Find the path
	std::vector<dim::Vector3> path;

	// Stop the timer
	auto end_time = std::chrono::high_resolution_clock::now();
	auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	// Print the elapsed time in milliseconds
	std::cout << "Pathfinding took " << elapsed_time.count() << " milliseconds.\n";
	int cos = 0;

 start_time = std::chrono::high_resolution_clock::now();


 std::thread thread_object(&Robot::action,robot);
 std::thread thread_object2(&Robot::action,robot2);
 thread_object.detach();
 thread_object2.detach();


	while (dim::Window::running)
	{

		// Check events
		sf::Event sf_event;
		while (dim::Window::poll_event(sf_event))
		{
			dim::Window::check_events(sf_event);
			scene.check_events(sf_event);
		}
		
		
	
		// Clear the screen
		dim::Window::clear();
		scene.clear();
		// Update interactive stuff
		dim::Window::update();
		scene.update();
		// Draw the objects
		int i = 0;
		for (auto& object : pyramid_objects)
		{
			if (i < robots + pyramid_count+1)
			{
				scene.draw(*object);
				i++;
			}
		}
		// Display the scenes to the window
		scene.display();

		// Display the window to the screen
		dim::Window::display();
	}

	// Close the main window and shut down the libraries
	dim::Window::close();
	closesocket(serverSocket);
	WSACleanup();
	return EXIT_SUCCESS;
}
