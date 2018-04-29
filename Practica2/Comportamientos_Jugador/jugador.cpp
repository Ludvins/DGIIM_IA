#include "./jugador.hpp"
#include "../include/motorlib/util.h"


int debug = false;
bool case2 = true;

void ComportamientoJugador::PintaPlan(list<Action> plan) {
	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			cout << "A ";
		}
		else if (*it == actTURN_R){
			cout << "D ";
		}
		else if (*it == actTURN_L){
			cout << "I ";
		}
		else {
			cout << "- ";
		}
		it++;
	}
	cout << endl;
}

void AnularMatriz(vector<vector<unsigned char> > &m){
	for (int i=0; i<m[0].size(); i++){
		for (int j=0; j<m.size(); j++){
			m[i][j]=0;
		}
	}
}

void ComportamientoJugador::VisualizaPlan(const estado &st, const list<Action> &plan){
  AnularMatriz(mapaConPlan);
	estado cst = st;

	auto it = plan.begin();
	while (it!=plan.end()){
		if (*it == actFORWARD){
			switch (cst.orientacion) {
				case 0: cst.fila--; break;
				case 1: cst.columna++; break;
				case 2: cst.fila++; break;
				case 3: cst.columna--; break;
			}
			mapaConPlan[cst.fila][cst.columna]=1;
		}
		else if (*it == actTURN_R){
			cst.orientacion = (cst.orientacion+1)%4;
		}
		else {
			cst.orientacion = (cst.orientacion+3)%4;
		}
		it++;
	}
}

int estimateDistance(const estado &n1, const estado &n2){

  if ( n1.fila == n2.fila && n1.columna == n2.columna) return 0;

  int coldif = abs(n2.columna - n1.columna);
  int fildif = abs(n2.fila - n1.fila);

  int spins = 2;

  //         |   |
  //    1    | 0 |     1
  //         |   |
  // ________|   |_________
  //    1      A       1
  // ________     __________
  //         |   |
  //    2    | 2 |     2
  //         |   |

  if(case2){
    switch(n1.orientacion){
    case 0:
      if (n1.columna == n2.columna){
        if (n2.fila < n1.fila) spins = 0;
        else spins = 2;
      }
      else if (n2.fila > n1.fila) spins = 2;
      else spins = 1;
      break;
    case 1:
      if (n1.fila == n2.fila){
        if (n2.columna > n1.columna) spins = 0;
        else spins = 2;
      }
      else if (n2.columna < n1.columna) spins = 2;
      else spins = 1;
      break;
    case 2:
      if (n1.columna == n2.columna){
        if (n2.fila > n1.fila) spins = 0;
        else spins = 2;
      }
      else if (n2.fila < n1.fila) spins = 2;
      else spins = 1;
      break;
    case 3:
      if (n1.fila == n2.fila){
        if (n2.columna < n1.columna) spins = 0;
        else spins = 2;
      }
      else if (n2.columna > n1.columna) spins = 2;
      else spins = 1;
      break;
    }
  }

  int md = coldif + fildif ;

  return md + spins;
}

class comp : public std::binary_function<estado, estado, bool>{
  std::reference_wrapper < std::map <estado, int, estadocomp> > fScore;
public:
  comp(map <estado, int, estadocomp>& a): fScore(a){}

  bool operator()(const estado& a, const estado& b) const {

    //if(debug) cout << "Comparando: "<< a << "  " << b << "   " << fScore.get().find(a)->second << " " << fScore.get().find(b)->second << endl;
    if (fScore.get().find(b)-> second == 0)  return true;

    if (fScore.get()[a] < fScore.get()[b]) return true;
    if (fScore.get()[a] > fScore.get()[b]) return false;
    return estadocomp()(a,b);

  }
};

bool estadocomp::operator() (const estado& lhs, const estado& rhs) const{
  if (lhs.fila < rhs.fila) return true;
  if (lhs.fila > rhs.fila) return false;
  if (lhs.columna < rhs.columna) return true;
  return false;
}

bool isPath(unsigned char c, bool b = true){
  bool ret = c == 'T' || c == 'S' || c == 'K' || (b && c == '?') ;
  return ret;
}

void reconstructPath(const map<estado, estado, estadocomp>& cameFrom, const estado& current, list<Action>& plan ){

  estado b = current;

  if (debug) cout << "[PathFinding; Lambda; reconstructPath]: Entra funcion con estado: " << b.fila << " " << b.columna << " " << b.orientacion << endl;

  for(auto a = cameFrom.find(b) ; a != cameFrom.end(); a=cameFrom.find(a->second)){
    if (debug) cout << "[PathFinding; Lambda; reconstructPath]: Estado anterior: " << a->second.fila << " " << a->second.columna << " " << a->second.orientacion <<endl;

    plan.push_back(actFORWARD);

    if ( (a->second.orientacion + 1)%4 == b.orientacion )
      plan.push_back(actTURN_R);
    else if ( (a->second.orientacion + 3)%4 == b.orientacion )
      plan.push_back(actTURN_L);
    else if ( (a->second.orientacion + 2)%4 == b.orientacion ){
      plan.push_back(actTURN_L);
      plan.push_back(actTURN_L);
    }

    b = a->second;
  }

}

template <typename T>
void getNeighbors(estado current, T& M, list<pair<estado, int>>& neighbors, bool b = true){

   int rowForward = current.fila + (current.orientacion - 1)%2; int colForward = current.columna - (current.orientacion - 2)%2;
   int rowRight = current.fila - (current.orientacion - 2)%2;   int colRight = current.columna - (current.orientacion - 1)%2;
   int rowLeft = current.fila + (current.orientacion - 2)%2;    int colLeft = current.columna + (current.orientacion - 1)%2;
   int rowBack = current.fila - (current.orientacion - 1)%2;    int colBack = current.columna + (current.orientacion - 2)%2;

   if(isPath(static_cast<char> (M [ rowForward ] [ colForward ]), b)){ //Forward
      estado neighbor { rowForward, colForward, current.orientacion};
      if (debug)
        cout << "[PathFinding]: Nodo Vecino 1: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      neighbors.push_back(make_pair(neighbor,1));
    }

   if(isPath(static_cast<char> (M [ rowRight ] [ colRight ] ), b)){ //Right
      //estado neighbor {current.fila, current.columna, (current.orientacion+1)%4};
      estado neighbor {rowRight, colRight, (current.orientacion+1)%4};
      if (debug)
        cout << "[PathFinding]: Nodo Vecino 2: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      neighbors.push_back(make_pair(neighbor,2));
    }

   if(isPath(static_cast<char>(M [ rowLeft ] [ colLeft ] ), b)){//Left
      //estado neighbor {current.fila, current.columna, (current.orientacion+3)%4};
      estado neighbor {rowLeft, colLeft, (current.orientacion+3)%4};
      if (debug)
        cout << "[PathFinding]: Nodo Vecino 3: " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      neighbors.push_back(make_pair(neighbor,2));
    }

   if (isPath(static_cast<char>(M [ rowBack ][ colBack ]), b)){ //Backwards
      estado neighbor = {rowBack, colBack, (current.orientacion+2)%4};
      if (debug)
        cout << "[PathFinding]: Nodo vecino 4. " << neighbor.fila << " " << neighbor.columna << " " << neighbor.orientacion << endl;
      neighbors.push_back(make_pair(neighbor,3));
   }
}


template <typename T>
bool a_star_algorithm(const estado& origen, const estado& destino, list<Action>& plan, T& M, bool b){

  map < estado, int, estadocomp> gScore;
  map < estado, int, estadocomp> fScore;
  set < estado, estadocomp > closedSet;
  set < estado, comp > openSet((comp(fScore)));
  map < estado, estado, estadocomp> cameFrom;

  static int a = 0;
  bool Bounded = false;
  if(debug) cout << "[PathFinding]: Destino: " << destino << endl;
  if(debug) cout << "[PathFinding]: Inicializa Estructuras" << endl;

  gScore[origen] = 0;

  fScore[origen] = estimateDistance(origen, destino);

  openSet.insert(origen);

  if (debug) cout << "[PathFinding]: Inserta valores iniciales" << endl;

  while (!openSet.empty()){
    estado current = *(openSet.begin());
    a++;
    if(debug) usleep(100000);
    if(debug) cout << "[PathFinding]: Iteracion bucle A* con fila " << current.fila << ", columna " << current.columna << " y orientacion " << current.orientacion << " y fScore " << fScore[current] << endl;

    if(current == destino) {
      if (debug) cout <<  a << " Nodos explorados" << endl;
      reconstructPath(cameFrom, current, plan);
      return true;
    }
    if (debug) cout << "[PathFinding]: No es el nodo buscado" << endl;
    openSet.erase(openSet.begin());
    if (debug) cout << "[PathFinding]: Nodo eliminado de nodos pendientes. " << endl;
    closedSet.insert(current);
    if (debug) cout << "[PathFinding]: Nodo insertado en nodos visitados "<< endl;

    // Lambda Function
    auto f = [&](estado neighbor, int dist){

      if (debug) cout << "[PathFinding; Lambda]: Entra función." << endl;
      if (!closedSet.count( neighbor )){ // Not in closedSet

        if (debug) cout << "[PathFinding; Lambda]: Vecino no explorado" << endl;
        /*if (!openSet.count( neighbor )){ // Not in openSet
            openSet.insert( neighbor );
            if (debug) cout << "[PathFinding; Lambda]: Vecino insertado en Lista de nodos pendientes" << endl;
            }*/

        if ( gScore.insert(make_pair(neighbor, INT_MAX)).second )
          if (debug) cout << "[PathFinding; Lambda]: Inicializado valor de gScore a inf." << endl;

        int tentative_gScore = gScore[current] + dist;

        if (debug) cout << "[PathFinding; Lambda]: Calculado posible nuevo gScore." << endl;

        if (tentative_gScore <  gScore[neighbor]) {

          if (debug) cout << "[PathFinding; Lambda]: Valor calculado supone una mejora." << endl;

          cameFrom.erase( neighbor );
          cameFrom.insert( std::pair <estado, estado> (neighbor, current));
          if (debug) cout << "[PathFinding; Lambda]: Insertado nuevo camino en cameFrom." << endl;

          gScore.erase( neighbor );
          gScore.insert( make_pair ( neighbor, tentative_gScore ) );
          if (debug) cout << "[PathFinding; Lambda]: Actualizado valor en gScore: " << gScore[neighbor] << endl;

          openSet.erase( neighbor );
          fScore.erase( neighbor );
          fScore[neighbor] = estimateDistance(neighbor, destino) + (Bounded ? 0 : tentative_gScore);

          if (debug) cout << "[PathFinding; Lambda]: Actualizado valor en fScore: " << fScore[neighbor] << endl;

          openSet.insert(neighbor);
          if (debug) cout << "[PathFinding; Lambda]: Actualizado posicion en openSet." << endl << "[PathFinding; Lambda]: Estado de openSet: " << endl;

          if(debug) for(auto it : openSet){
            cout << it.fila  << " " << it.columna << " " << it.orientacion << "  " << fScore.find(it)->second << endl;
          }
          if (debug)cout << endl;
        }
      }
    }; //END


    list<pair<estado,int>> neighbors;

    getNeighbors(current, M, neighbors, b);
    for(auto it : neighbors)
      f(it.first, it.second);


  }
  return false;
}

bool ComportamientoJugador::pathFinding(const estado &origen, const estado &destino, list<Action> &plan) {
  return a_star_algorithm <> (origen, destino, plan, mapaResultado, false);
}



estado ComportamientoJugador::interpretVision(int x, mapOfChar& m ){

  int matrix[15][2] = {{-1,-1},{-1,0},{-1,1},{-2,-2},{-2,-1},{-2,0},{-2,1},{-2,2},{-3,-3},{-3,-2},{-3,-1},{-3,0},{-3,1},{-3,2},{-3,3}};
  x = x-1;

  switch (brujula) {
  case 0:
    if (m.m[fil+matrix[x][0]][col+matrix[x][1]] == 'K')  return {fil+matrix[x][0], col+matrix[x][1], brujula};
    break;
  case 1:
    if (m.m[fil+matrix[x][1]][col-matrix[x][0]] == 'K') return {fil+matrix[x][1], col-matrix[x][0], brujula};
    break;
  case 2:
    if( m.m[fil-matrix[x][0]][col-matrix[x][1]] == 'K' ) return {fil-matrix[x][0], col-matrix[x][1], brujula};
    break;
  case 3:
    if ( m.m[fil-matrix[x][1]][col+matrix[x][0]] == 'K') return {fil-matrix[x][1], col+matrix[x][0], brujula};
    break;
  }

}
template <class T>
void ComportamientoJugador::saveVisibleMap(Sensores s, T& m){

  //estado neighbor {current.fila + (current.orientacion-1)%2, current.columna - (current.orientacion - 2)%2, current.orientacion};
  //cout << "[saveVisibleMap]: Posicion actual: " << fil << " " << col << " " << brujula << endl;
  m[fil][col] = s.terreno[0];

  //cout << s.terreno[9] << " " << s.terreno[10] << " " << s.terreno[11] << " " << s.terreno[12] << " " << s.terreno[13] << " " << s.terreno[14]  << " " << s.terreno[15] << endl;
  //cout << "  " << s.terreno[4] << " " << s.terreno[5] << " " << s.terreno[6] << " " << s.terreno[7] << " " << s.terreno[8] << endl;
  //cout << "    "  << s.terreno[1] << " " << s.terreno[2] << " " << s.terreno[3] << endl;
  //cout << "      " << s.terreno[0] << endl;


  int matrix[15][2] = {{-1,-1},{-1,0},{-1,1},{-2,-2},{-2,-1},{-2,0},{-2,1},{-2,2},{-3,-3},{-3,-2},{-3,-1},{-3,0},{-3,1},{-3,2},{-3,3}};

  switch (brujula) {
  case 0:
    for( int i = 0; i < 15; i++)
      m[fil+matrix[i][0]][col+matrix[i][1]] = s.terreno[i+1];
    break;
  case 1:
    for( int i = 0; i < 15; i++)
      m[fil+matrix[i][1]][col-matrix[i][0]] = s.terreno[i+1];
    break;
  case 2:
    for( int i = 0; i < 15; i++)
      m[fil-matrix[i][0]][col-matrix[i][1]] = s.terreno[i+1];
    break;

  case 3:
    for( int i = 0; i < 15; i++)
      m[fil-matrix[i][1]][col+matrix[i][0]] = s.terreno[i+1];
    break;
    }
  //cout << "[addToKnownMap]: Añadidos valores a la vista." << endl;

}

void ComportamientoJugador::RandomBehaviour(mapOfChar& knownMap){

  static int spins = 0;

  if (spins < 4){
    plan.push_back(actTURN_R);
    spins++;
    return;
  }

  estado forward = {fil + (brujula-1)%2, col - (brujula-2)%2, brujula};
  pairIntchar forwardp = knownMap.m[forward.fila][forward.columna];
  char forwardc = static_cast<char>(forwardp);
  int forwardi = static_cast<int>(forwardp);


  estado right = { fil - (brujula-2)%2, col - (brujula - 1)%2, (brujula+1)%4};
  pairIntchar rightp = knownMap.m[right.fila][right.columna];
  char rightc = static_cast<char>(rightp);
  int righti = static_cast<int>(rightp);


  estado left = {fil + (brujula-2)%2, col + (brujula - 1)%2, (brujula+3)%4};
  pairIntchar leftp = knownMap.m[left.fila][left.columna];
  char leftc = static_cast<char>(leftp);
  int lefti = static_cast<int>(leftp);

  plan.push_back(actFORWARD);

  if (!isPath(forwardc) && !isPath(rightc) && !isPath(leftc)){
    plan.push_back(actTURN_R);
    plan.push_back(actTURN_R);
    return;
  }

  if (isPath(forwardc)){

    if(!isPath(rightc) && isPath(leftc)){
        if (forwardi > lefti){
        plan.push_back(actTURN_L);
        return;

      }
    }

    if (!isPath(leftc) && isPath(rightc)){
      if (forwardi > righti){
        plan.push_back(actTURN_R);
        return;
      }
    }

    if (isPath(leftc) && isPath(rightc)){

      if(lefti < forwardi && lefti <= righti){
        plan.push_back(actTURN_L);
        return;
    }
      if(righti < forwardi && righti < lefti){
        plan.push_back(actTURN_R);
        return;
      }
    }
  }
  else{
    if (isPath(leftc) && isPath(rightc)){
      if (lefti >= righti){
        plan.push_back(actTURN_R);
        return;
      }
      plan.push_back(actTURN_L);
      return;
    }

    if(isPath(leftc) && !isPath(rightc)){
      plan.push_back(actTURN_L);
      return;
    }
    if(isPath(rightc) && !isPath(leftc)){
      plan.push_back(actTURN_R);
      return;
    }
  }
}



// Updates row (r) and column (c) using an Action (a) and orientation (b).
void updatePosition(Action a, int& r, int& c, int& b){

  switch(a){
  case actFORWARD:
    r += (b - 1)%2;
    c -= (b - 2)%2;
    break;
  case actTURN_L:
    b = (b + 3)%4;
    break;
  case actTURN_R:
    b = (b + 1)%4;
    break;
  default:
    break;
  }

}

bool ComportamientoJugador::canSeeK(Sensores s, estado& k, mapOfChar& m){
  for (int i = 0; i < 15 ; i++){
    if (s.terreno[i] == 'K'){
      k = interpretVision(i, m);
      //cout << i << " "<< k << endl;
      return true;
    }
  }
  return false;
}

bool ComportamientoJugador::inicializeCoordenates(Sensores s, bool a = true){
  static bool alreadyInitialized = false;
  if (alreadyInitialized && a) return false;
  fil = s.mensajeF;
  col = s.mensajeC;
  alreadyInitialized = true;

  if (fil == -1){
    fil = 0; col = 0;
    return false;
  }
  return true;

}

bool ComportamientoJugador::objetiveHaschanged(estado obj, Sensores sensores){
  return (obj.fila != sensores.destinoF || obj.columna != sensores.destinoC);
}


bool ComportamientoJugador::thereIsPathForK (const estado k, mapOfChar& knownMap){
  //cout << "[thereIsPathForK]. entra funcion" << endl;
  return a_star_algorithm <> ({fil, col, brujula}, k, plan, knownMap.m, false);
}


void ComportamientoJugador::copyKnownMap(mapOfChar& m, Sensores s){

  for(int i = 0; i < mapaResultado.size(); i++){
    mapaResultado[0][i] = 'P';
    mapaResultado[1][i] = 'P';
    mapaResultado[2][i] = 'P';
    mapaResultado[mapaResultado.size()-1][i] = 'P';
    mapaResultado[mapaResultado.size()-2][i] = 'P';
    mapaResultado[mapaResultado.size()-3][i] = 'P';
    mapaResultado[i][0] = 'P';
    mapaResultado[i][1] = 'P';
    mapaResultado[i][2] = 'P';
    mapaResultado[i][mapaResultado.size()-1] = 'P';
    mapaResultado[i][mapaResultado.size()-2] = 'P';
    mapaResultado[i][mapaResultado.size()-3] = 'P';
  }

  int rowdiff = s.mensajeF - fil;
  int coldiff = s.mensajeC - col;

  for(auto it: m.m){
    int row = it.first;
    for (auto it2 : it.second){
      int column = it2.first;
      //cout << "[copyKnownMap]: Fila: " << rowdiff + row << " columna: " << coldiff + column << " " << static_cast<char>(it2.second) << endl;
      mapaResultado[rowdiff + row][coldiff+column] = static_cast<char>(it2.second);
      //cout << "[copyKnownMap]: Copiado" << endl;
    }
  }
}

enum state{
  LOOKINGFORK,
  GOINGFORK,
  KNOWINGCOORDENATES,
  LOOKINGFOROBJETIVE,
  ONK,
  KNOWEXACTCOORDENATESANDMAP,
};

ostream& operator<< (ostream& os, state s){
  switch(s){
  case LOOKINGFORK:
    os << "LOOKING FOR K";
    break;
  case GOINGFORK:
    os << "GOING FOR K";
    break;
  case KNOWINGCOORDENATES:
    os << "KNOWING COORDENATES";
    break;
  case LOOKINGFOROBJETIVE:
    os << "LOOKING FOR OBJETIVE";
    break;
  case KNOWEXACTCOORDENATESANDMAP:
    os << "KNOWING EXACT COORDENATES AND MAP";
    break;
  case ONK:
    os << "ONK";
    break;
  }
  return os;
}

Action ComportamientoJugador::think(Sensores sensores) {

  static bool oneobjetive = false;
  destino = {sensores.destinoF, sensores.destinoC, 0};
  static int it = 0;
  static state state = LOOKINGFORK;
  static estado k;
  static mapOfChar knownMap;
  if (oneobjetive) return actTURN_L;

  if (inicializeCoordenates(sensores)){
    state = KNOWEXACTCOORDENATESANDMAP;
  }


  if (fil == destino.fila && col == destino.columna && (state == LOOKINGFOROBJETIVE || state == KNOWINGCOORDENATES)){
    cout << "[Think]: En el objetivo. " <<endl;
    //oneobjetive = true;
   return actIDLE;
  }


  if (objetiveHaschanged(destino, sensores)){
    cout << "[Think]: El objetivo ha cambiad, vaciando plan..." << endl;
    plan.clear();
    state = KNOWINGCOORDENATES;
    destino = {sensores.destinoF, sensores.destinoC, 0};
    }

  cout << "[Think]: Estado actual: " << state << endl;

  if(plan.empty()){
    cout << "[Think]: El plan esta vacio." << endl;

      switch (state)
        {
        case LOOKINGFORK:

          saveVisibleMap <> (sensores, knownMap.m);

          if(!canSeeK(sensores,k,knownMap)){
            RandomBehaviour(knownMap);
            break;
          }
          cout << "[Think]: Puedo ver la k en " << k << endl;
          state = GOINGFORK;

        case GOINGFORK:

          if (thereIsPathForK(k, knownMap)){

            cout << "[Think]: Hay un camino hacia la K." <<endl;
            state = ONK;
          }
          else{
            state = LOOKINGFORK;
            RandomBehaviour(knownMap);
          }
          break;

        case ONK:

          copyKnownMap(knownMap, sensores);
          inicializeCoordenates(sensores, false);
          state = KNOWINGCOORDENATES;

        case KNOWINGCOORDENATES:

          if(a_star_algorithm <> ({fil, col, brujula}, destino, plan, mapaResultado, true)){
            //cout << "[Think]: Hay un camino hacia el objetivo" << endl;
            break;
          }
          state = LOOKINGFOROBJETIVE;
          //cout << "No hay camino al objetivo." << endl;

        case LOOKINGFOROBJETIVE:
          state = KNOWINGCOORDENATES;
          break;

        case KNOWEXACTCOORDENATESANDMAP:

          cout << "[Think]: Fila inicial: " << fil << "\ncolumna inicial: "<< col  << "\nbrujula: "<< brujula << endl;
          cout << "[Think]: Destino " << sensores.destinoF << " " << sensores.destinoC << endl;
          cout << "[Think]: Entra funcion pathfinding" << endl;
          estado orig = {fil, col, brujula};
          pathFinding(orig, destino, plan);
          break;
        }


  }

  if(state == LOOKINGFOROBJETIVE || state == KNOWINGCOORDENATES) saveVisibleMap<> (sensores, mapaResultado);
  else
    saveVisibleMap<> (sensores, knownMap.m);

  knownMap.count(fil,col);
  Action ret;
  if (plan.empty()){

    //cout << "Plan vacio: Número de pasos: " << it << endl;
    ret = actIDLE;
  }
  else{


    ret = plan.back();
    // If prediction isnt good we got 3 cases to take care of, this shouldnt be here but doesnt have much time to fix this. 
    //
    if (ret == actFORWARD && ( state == LOOKINGFOROBJETIVE || state == KNOWINGCOORDENATES) && !isPath(mapaResultado[fil+(brujula-1)%2][col-(brujula-2)%2], false)){
        plan.clear();
        a_star_algorithm <> ({fil, col, brujula}, destino, plan, mapaResultado, true);
        ret = plan.back();
    }
    //
    if (ret == actTURN_R && (state == LOOKINGFOROBJETIVE || state == KNOWINGCOORDENATES) && !isPath(mapaResultado[fil-(brujula-2)%2][col-(brujula-1)%2], false)){
      plan.clear();
      a_star_algorithm <> ({fil, col, brujula}, destino, plan, mapaResultado, true);
      ret = plan.back();
    }
    //
    if (ret == actTURN_L && (state == LOOKINGFOROBJETIVE || state == KNOWINGCOORDENATES) && !isPath(mapaResultado[fil+(brujula-2)%2][col+(brujula-1)%2], false)){
      plan.clear();
      a_star_algorithm <> ({fil, col, brujula}, destino, plan, mapaResultado, true);
      ret = plan.back();
    }

    if (ret == actFORWARD && sensores.superficie[2] == 'a'){ // Avoid Aldeano
      cout << "[Think]: Evita colision con aldeano." << endl;;
      return actIDLE;
    }
    else plan.pop_back();
  }

  updatePosition(ret , fil, col, brujula);

  cout << "[Think]: Posicion Actual: \n\tFila: " << fil << "\n\tColumna: " << col << "\n\tOrientacion: " << brujula << endl;

  it++;
  return ret;

}


int ComportamientoJugador::interact(Action accion, int valor){
  return false;
}
