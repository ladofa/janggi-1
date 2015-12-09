#include <cmath>
#include <algorithm>

#include "janggi.h"
#include "node.h"
#include "defines.h"

const char* UnitIDChar[IDSize] = {
  "HG", "HC", "HM", "HS", "HP", "Hs", "HJ",
  "CG", "CC", "CM", "CS", "CP", "Cs", "CJ",
};

float Pos::Distance(int x, int y)
{
  int diff_x = this->x - x;
  int diff_y = this->y - y;
  return sqrt((float)(diff_x*diff_x + diff_y*diff_y));
}

Janggi::Janggi() {
  for (int y = 0; y < kStageHeight; y++) {
    for (int x = 0; x < kStageWidth; x++) {
      stage_[y][x] = -1;
    }
  }
  SetStage(MSSMSMSM);
}

bool Janggi::CheckValidPos(Pos pos)
{
  if (pos.x < 0 || pos.x >= kStageWidth || pos.y < 0 || pos.y >= kStageHeight)
    return false;
  return true;
}

void Janggi::SetStage(StageID stage_id)
{
  switch (stage_id) {
  case MSSMSMSM:    
    stage_[0][0] = HC;  stage_[0][1] = HM;  stage_[0][2] = HS;
    stage_[0][3] = Hs;  stage_[0][5] = Hs;  stage_[0][6] = HS;
    stage_[0][7] = HM;  stage_[0][8] = HC;  stage_[2][1] = HP;
    stage_[2][7] = HP;  stage_[3][0] = HJ;  stage_[3][2] = HJ;
    stage_[3][4] = HJ;  stage_[3][6] = HJ;  stage_[3][8] = HJ;
    stage_[1][4] = HG;

    stage_[9][0] = CC;  stage_[9][1] = CS;  stage_[9][2] = CM;
    stage_[9][3] = Cs;  stage_[9][5] = Cs;  stage_[9][6] = CS;
    stage_[9][7] = CM;  stage_[9][8] = CC;  stage_[7][1] = CP;
    stage_[7][7] = CP;  stage_[6][0] = CJ;  stage_[6][2] = CJ;
    stage_[6][4] = CJ;  stage_[6][6] = CJ;  stage_[6][8] = CJ;
    stage_[8][4] = CG;
    break;
  }
}

const char* Janggi::GetUnitID(Pos pos)
{
  if (!CheckValidPos(pos))
    return NULL;

  if (stage_[pos.y][pos.x] >= 0)
    return UnitIDChar[stage_[pos.y][pos.x]];
  else {
    return "--";
  }
}

void Janggi::Show(Pos prev)
{
  cout << "   ";
  for (int x = 0; x < kStageWidth; x++)
    cout << x << "   ";
  cout << endl;

  for (int y = 0; y < kStageHeight; y++) {    
    cout << y << " ";
    for (int x = 0; x < kStageWidth; x++) {
      if (x == prev.x && y == prev.y ) 
        cout << "##";
      else
        cout << GetUnitID(Pos(x, y));
      if (x != kStageWidth - 1)
        cout << "  ";
    }
    if (y != kStageHeight - 1) {
      cout << endl << "  ";
      for (int x = 0; x < kStageWidth; x++) {
        cout << "    ";
      }
      cout << endl;
    }
  }
  cout << endl << "====================================" << endl;
}

bool Janggi::PerformAction(Action action)
{
  Pos curr = action.curr;
  Pos next = action.next;
  if (!CheckValidPos(curr) || !CheckValidPos(next) || stage_[curr.y][curr.x] < 0)
    return false;

  vector<Pos> candidates;
  MovableCanditates(stage_, curr, candidates);

#if DEBUG_MODE
  for (int i = 0; i < candidates.size(); i++) {
    cout << candidates[i].x << ", " << candidates[i].y << endl;
  }
#endif
    
  if (candidates.size() != 0) {
    for (unsigned int i = 0; i < candidates.size(); i++) {
      if (candidates[i].x == next.x && candidates[i].y == next.y) {
        printf("%d,%d->%d,%d\n", curr.x, curr.y, next.x, next.y);
        MakeNextStage(action, stage_);      
        return true;        
      }
    }    
  }  
  return false;    
}

void Janggi::MakeNextStage(Action action, int stage[kStageHeight][kStageWidth])
{
  stage[action.next.y][action.next.x] = stage[action.curr.y][action.curr.x];
  stage[action.curr.y][action.curr.x] = -1;
}

void Janggi::MovableCanditates(int stage[kStageHeight][kStageWidth], Pos pos, vector<Pos>& candidates)
{
  candidates.clear();
  int curr_id = stage[pos.y][pos.x];

  if (curr_id < 0) {
    return;
  }
  else if (curr_id == HG || curr_id == CG 
          || curr_id == Hs || curr_id == Cs) {
    MoveGung(stage, pos, candidates);
  }
  else if (curr_id == HC || curr_id == CC) {
    MoveCha(stage, pos, candidates);
  }
  else if (curr_id == HM || curr_id == CM) {
    MoveMa(stage, pos, candidates);
  }
  else if (curr_id == HS || curr_id == CS) {
    MoveSang(stage, pos, candidates);
  }
  else if (curr_id == HP || curr_id == CP) {
    MovePo(stage, pos, candidates);
  }  
  else if (curr_id == HJ || curr_id == CJ) {
    MoveJol(stage, pos, candidates);
  }
}

void Janggi::MoveGung(int stage[kStageHeight][kStageWidth], Pos pos, vector<Pos>& candidates)
{
  int curr_id = stage[pos.y][pos.x];
  int nx, ny;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      nx = pos.x + dx;
      ny = pos.y + dy;
      if ((pos.x == 3 && (pos.y == 1 || pos.y == 8)) ||
        (pos.x == 4 && (pos.y == 0 || pos.y == 2 || pos.y == 7 || pos.y == 9)) ||
        (pos.x == 5 && (pos.y == 1 || pos.y == 8))) {
        // movable to only up, right, left, down side
        if (pos.Distance(nx, ny) > 1)
          continue;
      }
      if (nx >= 3 && nx <= 5
        && ((ny >= 0 && ny <= 2) || (ny >= 7 && ny <= 9))) {
        if (curr_id <= 6) {
          if (stage[ny][nx]<0 || stage[ny][nx]>6) {
            candidates.push_back(Pos(nx, ny));
          }
        }
        else {
          if (stage[ny][nx]<0 || stage[ny][nx] <= 6) {
            candidates.push_back(Pos(nx, ny));
          }
        }
      }
    }
  }
}

void Janggi::MoveCha(int stage[kStageHeight][kStageWidth], Pos pos, vector<Pos>& candidates)
{
  int curr_id = stage[pos.y][pos.x];

  // up
  if (pos.y>0) {
    for (int y = pos.y - 1; y >= 0; y--) {
      if (curr_id <= 6) {
        if (stage[y][pos.x] < 0) {
          candidates.push_back(Pos(pos.x, y));
        }
        else {
          if (stage[y][pos.x] > 6) {
            candidates.push_back(Pos(pos.x, y));            
          }
          break;
        }
      }
      else {
        if (stage[y][pos.x] < 0) {
          candidates.push_back(Pos(pos.x, y));
        }
        else {
          if (stage[y][pos.x] <= 6) {
            candidates.push_back(Pos(pos.x, y));
          }
          break;
        }
      }
    }
  }
  // down
  if (pos.y<kStageHeight - 1) {
    for (int y = pos.y + 1; y < kStageHeight; y++) {
      if (curr_id <= 6) {
        if (stage[y][pos.x] < 0) {
          candidates.push_back(Pos(pos.x, y));
        }
        else {
          if (stage[y][pos.x] > 6) {
            candidates.push_back(Pos(pos.x, y));            
          }
          break;
        }
      }
      else {
        if (stage[y][pos.x] < 0) {
          candidates.push_back(Pos(pos.x, y));
        }
        else {
          if (stage[y][pos.x] <= 6) {
            candidates.push_back(Pos(pos.x, y));            
          }
          break;
        }
      }
    }
  }
  // right
  if (pos.x<kStageWidth - 1) {
    for (int x = pos.x + 1; x < kStageWidth; x++) {
      if (curr_id <= 6) {
        if (stage[pos.y][x] < 0) {
          candidates.push_back(Pos(x, pos.y));
        }
        else {
          if (stage[pos.y][x] > 6) {
            candidates.push_back(Pos(x, pos.y));            
          }
          break;
        }
      }
      else {
        if (stage[pos.y][x] < 0) {
          candidates.push_back(Pos(x, pos.y));
        }
        else {
          if (stage[pos.y][x] <= 6) {
            candidates.push_back(Pos(x, pos.y));            
          }
          break;
        }
      }
    }
  }
  // left
  if (pos.x>0) {
    for (int x = pos.x - 1; x >= 0; x--) {
      if (curr_id <= 6) {
        if (stage[pos.y][x] < 0) {
          candidates.push_back(Pos(x, pos.y));
        }
        else {
          if (stage[pos.y][x] > 6) {
            candidates.push_back(Pos(x, pos.y));            
          }
          break;
        }
      }
      else {
        if (stage[pos.y][x] < 0) {
          candidates.push_back(Pos(x, pos.y));
        }
        else {
          if (stage[pos.y][x] <= 6) {
            candidates.push_back(Pos(x, pos.y));            
          }
          break;
        }
      }
    }
  }

  // diagonal
  if (pos.x >= 3 && pos.x <= 5
    && ((pos.y >= 0 && pos.y <= 2) || (pos.y >= 7 && pos.y <= 9))) {
    int nx, ny;
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        nx = pos.x + dx;
        ny = pos.y + dy;
        if ((pos.x == 3 && (pos.y == 1 || pos.y == 8)) ||
          (pos.x == 4 && (pos.y == 0 || pos.y == 2 || pos.y == 7 || pos.y == 9)) ||
          (pos.x == 5 && (pos.y == 1 || pos.y == 8))) {
          // movable to only up, right, left, down side
          if (pos.Distance(nx, ny) > 1)
            continue;
        }
        if (nx >= 3 && nx <= 5
          && ((ny >= 0 && ny <= 2) || (ny >= 7 && ny <= 9))) {
          if (curr_id <= 6) {
            if (stage[ny][nx]<0 || stage[ny][nx]>6) {
              candidates.push_back(Pos(nx, ny));
            }
          }
          else {
            if (stage[ny][nx]<0 || stage[ny][nx] <= 6) {
              candidates.push_back(Pos(nx, ny));
            }
          }
        }
      }
    }
  }
}

void Janggi::MovePo(int stage[kStageHeight][kStageWidth], Pos pos, vector<Pos>& candidates)
{
  int curr_id = stage[pos.y][pos.x];

  // up
  if (pos.y > 1) {
    bool movable = false;
    for (int y = pos.y - 1; y >= 0; y--) {
      if (movable) {
        if (stage[y][pos.x] < 0) {
          candidates.push_back(Pos(pos.x, y));          
        }
        else if (stage[y][pos.x] == HP || stage[y][pos.x] == CP) {
          break;
        }
        else {
          if ((curr_id<=6 && stage[y][pos.x]>6) || (curr_id>6 && stage[y][pos.x]<=6))
            candidates.push_back(Pos(pos.x, y));
          break;
        }
      }
      else {
        if (stage[y][pos.x] == HP || stage[y][pos.x] == CP) {
          break;
        }
        else if (stage[y][pos.x] > 0) {
          movable = true;
        }
      }
    }
  }

  // down
  if (pos.y < kStageHeight-2) {
    bool movable = false;
    for (int y = pos.y + 1; y <kStageHeight; y++) {
      if (movable) {
        if (stage[y][pos.x] < 0) {
          candidates.push_back(Pos(pos.x, y));
        }
        else if (stage[y][pos.x] == HP || stage[y][pos.x] == CP) {
          break;
        }
        else {
          if ((curr_id <= 6 && stage[y][pos.x]>6) || (curr_id>6 && stage[y][pos.x] <= 6))
            candidates.push_back(Pos(pos.x, y));
          break;
        }
      }
      else {
        if (stage[y][pos.x] == HP || stage[y][pos.x] == CP) {
          break;
        }
        else if (stage[y][pos.x] > 0) {
          movable = true;
        }
      }
    }
  }

  // right
  if (pos.x < kStageWidth - 2) {
    bool movable = false;
    for (int x = pos.x + 1; x <kStageWidth; x++) {
      if (movable) {
        if (stage[pos.y][x] < 0) {
          candidates.push_back(Pos(x, pos.y));
        }
        else if (stage[pos.y][x] == HP || stage[pos.y][x] == CP) {
          break;
        }
        else {
          if ((curr_id <= 6 && stage[pos.y][x]>6) || (curr_id>6 && stage[pos.y][x] <= 6))
            candidates.push_back(Pos(x, pos.y));
          break;
        }
      }
      else {
        if (stage[pos.y][x] == HP || stage[pos.y][x] == CP) {
          break;
        }
        else if (stage[pos.y][x] > 0) {
          movable = true;
        }
      }
    }
  }

  // left
  if (pos.x > 1) {
    bool movable = false;
    for (int x = pos.x - 1; x >= 0; x--) {
      if (movable) {
        if (stage[pos.y][x] < 0) {
          candidates.push_back(Pos(x, pos.y));
        }
        else if (stage[pos.y][x] == HP || stage[pos.y][x] == CP) {
          break;
        }
        else {
          if ((curr_id <= 6 && stage[pos.y][x]>6) || (curr_id>6 && stage[pos.y][x] <= 6))
            candidates.push_back(Pos(x, pos.y));
          break;
        }
      }
      else {
        if (stage[pos.y][x] == HP || stage[pos.y][x] == CP) {
          break;
        }
        else if (stage[pos.y][x] > 0) {
          movable = true;
        }
      }
    }
  }

  // diagonal  
  Pos next;
  if (stage[4][1] > 0 && stage[4][1] != HP && stage[4][1] != CP) {
    if (pos.x == 3 && pos.x == 0) {
      next = Pos(5, 2);
      if ((curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6))
        candidates.push_back(next);
    }
    else if (pos.x == 3 && pos.x == 2) {
      next = Pos(5, 0);
      if ((curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6))
        candidates.push_back(next);
    }
    else if (pos.x == 5 && pos.x == 0) {
      next = Pos(3, 2);
      if ((curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6))
        candidates.push_back(next);
    }
    else if (pos.x == 5 && pos.x == 2) {
      next = Pos(3, 0);
      if ((curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6))
        candidates.push_back(next);
    }
  }
  else if (stage[4][8] > 0 && stage[4][8] != HP && stage[4][8] != CP) {
    if (pos.x == 3 && pos.x == 7) {
      next = Pos(5, 9);
      if ((curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6))
        candidates.push_back(next);
    }
    else if (pos.x == 3 && pos.x == 9) {
      next = Pos(5, 7);
      if ((curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6))
        candidates.push_back(next);
    }
    else if (pos.x == 5 && pos.x == 7) {
      next = Pos(3, 9);
      if ((curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6))
        candidates.push_back(next);
    }
    else if (pos.x == 5 && pos.x == 9) {
      next = Pos(3, 7);
      if ((curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6))
        candidates.push_back(next);
    }
  }
}

void Janggi::MoveMa(int stage[kStageHeight][kStageWidth], Pos pos, vector<Pos>& candidates)
{
  int curr_id = stage[pos.y][pos.x];
  Pos next;
  if (pos.y > 1) {
    // up-left  
    next = Pos(pos.x - 1, pos.y - 2);
    if (stage[pos.y - 1][pos.x] < 0 && next.x >= 0 && ((curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);
    // up-right
    next = Pos(pos.x + 1, pos.y - 2);
    if (stage[pos.y - 1][pos.x] < 0 && next.x < kStageWidth && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);
  }  
  if (pos.x < kStageWidth - 2) {
    // right-up
    next = Pos(pos.x + 2, pos.y + 1);    
    if (stage[pos.y][pos.x + 1] < 0 && next.y <kStageHeight && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);

    // right-down
    next = Pos(pos.x + 2, pos.y - 1);
    if (stage[pos.y][pos.x + 1] < 0 && next.y >= 0 && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);
  }  
  if (pos.y < kStageHeight - 2) {
    // down-left
    next = Pos(pos.x - 1, pos.y + 2);
    if (stage[pos.y + 1][pos.x] < 0 && next.x >= 0 && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);

    // down-right
    next = Pos(pos.x + 1, pos.y + 2);
    if (stage[pos.y + 1][pos.x] < 0 && next.x < kStageWidth && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);
  }  
  if (pos.x > 1) {
    // left-up
    next = Pos(pos.x - 2, pos.y + 1);
    if (stage[pos.y][pos.x - 1] < 0 && next.y <kStageHeight && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);

    // left-down
    next = Pos(pos.x - 2, pos.y - 1);
    if (stage[pos.y][pos.x - 1] < 0 && next.y >= 0 && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);
  }  
}

void Janggi::MoveSang(int stage[kStageHeight][kStageWidth], Pos pos, vector<Pos>& candidates)
{
  int curr_id = stage[pos.y][pos.x];
  Pos next;
  if (pos.y > 2) {
    // up-left      
    next = Pos(pos.x - 2, pos.y - 3);
    if (pos.x > 1 && stage[pos.y - 1][pos.x]<0 && stage[pos.y - 2][pos.x - 1] < 0
      && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);

    // up-right    
    next = Pos(pos.x + 2, pos.y - 3);
    if (pos.x < kStageWidth - 2  && stage[pos.y - 1][pos.x]<0 && stage[pos.y - 2][pos.x + 1] < 0
      && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);
  }
  if (pos.x < kStageWidth - 3) {
    // right-up
    next = Pos(pos.x + 3, pos.y - 2);
    if (pos.y > 1 && stage[pos.y][pos.x + 1]<0 && stage[pos.y - 1][pos.x + 2] < 0
      && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);

    // right-down
    next = Pos(pos.x + 3, pos.y + 2);
    if (pos.y < kStageHeight - 2 && stage[pos.y][pos.x + 1]<0 && stage[pos.y + 1][pos.x + 2] < 0
      && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);

  }
  if (pos.y < kStageHeight - 3) {
    // down-left
    next = Pos(pos.x - 2, pos.y + 3);    
    if (pos.x > 1 && stage[pos.y + 1][pos.x]<0 && stage[pos.y + 2][pos.x - 1] < 0
      && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);

    // down-right
    next = Pos(pos.x + 2, pos.y + 3);    
    if (pos.x < kStageWidth - 2 && stage[pos.y + 1][pos.x]<0 && stage[pos.y + 2][pos.x + 1] < 0
      && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);
  }
  if (pos.x > 2) {
    // left-up
    next = Pos(pos.x - 3, pos.y - 2);
    if (pos.y > 1 && stage[pos.y][pos.x - 1]<0 && stage[pos.y - 1][pos.x - 2] < 0
      && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);

    // left-down
    next = Pos(pos.x - 3, pos.y + 2);
    if (pos.y < kStageHeight - 2 && stage[pos.y][pos.x - 1]<0 && stage[pos.y + 1][pos.x - 2] < 0
      && (stage[next.y][next.x] < 0 || (curr_id <= 6 && stage[next.y][next.x]>6) || (curr_id>6 && stage[next.y][next.x] <= 6)))
      candidates.push_back(next);
  }
}

void Janggi::MoveJol(int stage[kStageHeight][kStageWidth], Pos pos, vector<Pos>& candidates)
{
  int curr_id = stage[pos.y][pos.x];
  
  int nx, ny;
  // down-ward
  //if ((curr_id <= 6 && standard_position) || curr_id >6 && !standard_position) {
  if (curr_id <= 6) {
    for (int dy = 0; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        nx = pos.x + dx;
        ny = pos.y + dy;
        if (nx >= 0 && nx < kStageWidth && ny >= 0 && ny < kStageHeight && (stage[ny][nx] < 0 || (curr_id <= 6 && stage[ny][nx] > 6) || (curr_id > 6 && stage[ny][nx] <= 6))) {
          if ((pos.x == 3 && pos.y == 7 && nx == 4 && ny == 8) ||
            (pos.x == 5 && pos.y == 7 && nx == 4 && ny == 8) ||
            (pos.x == 4 && pos.y == 8 && nx == 3 && ny == 9) ||
            (pos.x == 4 && pos.y == 8 && nx == 5 && ny == 9) || pos.Distance(nx, ny) <= 1.1) {
            candidates.push_back(Pos(nx, ny));
          }
        }
      }
    }
  }
  // up-ward
  else {
    for (int dy = -1; dy <= 0; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        nx = pos.x + dx;
        ny = pos.y + dy;
        if (nx >= 0 && nx < kStageWidth && ny >= 0 && ny < kStageHeight && (stage[ny][nx] < 0 || (curr_id <= 6 && stage[ny][nx] > 6) || (curr_id > 6 && stage[ny][nx] <= 6))) {
          if ((pos.x == 3 && pos.y == 2 && nx == 4 && ny == 1) ||
            (pos.x == 5 && pos.y == 2 && nx == 4 && ny == 1) ||
            (pos.x == 4 && pos.y == 1 && nx == 3 && ny == 0) ||
            (pos.x == 4 && pos.y == 1 && nx == 5 && ny == 0) || pos.Distance(nx, ny) <= 1.1) {
            candidates.push_back(Pos(nx, ny));
          }
        }
      }
    }
  }
}

//return cho's score relative to han's score
//if return value is 0, the score is tied
//if return value is positive, cho is ahead of han
//if return value is negative, han is ahead of cho
int Janggi::Evaluate(int stage[kStageHeight][kStageWidth])
{
  int score_cho = 0, score_han = 0;

  for (int y=0 ; y<kStageHeight ; y++) {    
    for (int x=0 ; x<kStageWidth ; x++) {
      int val = stage[y][x];
      if (val >= 0) {
        if (val <= 6) 
          score_han += POINT[val];
        else
          score_cho += POINT[val-7];
      }
    }
  }

  return score_cho - score_han;
}

const Action Janggi::CalculateNextAction(Turn turn)
{
  Action action;   
  
  //mini-max algorithm
  const int maxDepth = 3;  
  Node* currentNode = new Node(stage_, 0, Janggi::Evaluate(stage_), turn, Action());  
  action = currentNode->CalculateMiniMaxAction(maxDepth, true);

  delete currentNode; 
  currentNode = NULL;

  return action;
}
