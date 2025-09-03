#pragma once
#include <Eigen/Dense>

class StateSpaceController {
public:
    StateSpaceController() {
        // Constructor implementation
    }

    void setState(const Eigen::VectorXd& state) {
        currentState = state;
    }

    Eigen::VectorXd getState() const {
        return currentState;
    }

    void update(const Eigen::VectorXd& controlInput) {
        // Update the state based on the control input
        currentState += controlInput;
    }

private:
    Eigen::VectorXd currentState;
    Eigen::MatrixXd A, B, C, D; // matrices de estado
};
