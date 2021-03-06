/******************************************************************************
 *                                                                            *
 *             Copyright (C) 2016 Mogi, LLC - All Rights Reserved             *
 *                            Author: Matt Bunting                            *
 *                                                                            *
 *            This program is distributed under the LGPL, version 2           *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU Lesser General Public License              *
 *   version 2.1 as published by the Free Software Foundation;                *
 *                                                                            *
 *   See license in root directory for terms.                                 *
 *   https://github.com/mogillc/nico/tree/master/edisonLibmogiPackage/libmogi *
 *                                                                            *
 *****************************************************************************/

#include "statechart.h"

using namespace Mogi;
using namespace StateChart;

State::State(std::string label) :
		parent(NULL), label(label) {
}

State::~State() {
	for (std::vector<Transition*>::iterator it = outgoingTransitions.begin();
			it != outgoingTransitions.end(); it++) {
		delete *it;
	}
	for (std::map<Event*, CallbackAction*>::iterator it = _events.begin();
			it != _events.end(); it++) {
		delete it->second;
	}
}

void State::notify(Event* event) {
	if (parent->currentState == this) {
		actionEvent(event);
	}
}

Transition* State::addTransition(State* dst) {
	Transition* transition = new Transition(this, dst);
	outgoingTransitions.push_back(transition);
	return transition;
}

void State::removeTransition(Transition* transition) {
	for (std::vector<Transition*>::iterator it = outgoingTransitions.begin();
			it != outgoingTransitions.end(); it++) {
		if (*it == transition) {
			delete *it;
			it = outgoingTransitions.erase(it);
			break;
		}
	}
}

void State::setCallbackEntry(actionCallback cb, void* parameter) {
	_entry.cb = cb;
	_entry.parameter = parameter;
}
void State::setCallbackDo(actionCallback cb, void* parameter) {
	_do.cb = cb;
	_do.parameter = parameter;
}
// void State::setCallbackEvent( actionCallback cb, void* parameter ) {
//	_event.cb = cb;
//	_event.parameter = parameter;
//}
void State::setCallbackEvent(Event* event, actionCallback cb, void* parameter) {
	//	if (_events[event] != NULL) {
	//		return;
	//	}
	for (std::map<Event*, CallbackAction*>::iterator it = _events.begin();
			it != _events.end(); it++) {
		if (it->first == event) {
			//			if (it->second->cb == cb &&
			//				it->second->parameter == parameter) {
			it->second->cb = cb;
			it->second->parameter = parameter;
			return;
			//			}
		}
	}
	CallbackAction* callback = new CallbackAction;
	callback->cb = cb;
	callback->parameter = parameter;
	_events[event] = callback;

	event->addObserver(this);
}
void State::setCallbackExit(actionCallback cb, void* parameter) {
	_exit.cb = cb;
	_exit.parameter = parameter;
}

std::vector<Transition*> State::getOutgoingTransitions() {
	return outgoingTransitions;
}

void State::actionEntry() {
	_entry.cb(_entry.parameter);
}
void State::actionDo() {
	_do.cb(_do.parameter);
}
void State::actionEvent(Event* event) {
	CallbackAction* callback = _events[event];
	if (callback != NULL) {
		callback->cb(callback->parameter);
	}
}
void State::actionExit() {
	_exit.cb(_exit.parameter);
}

std::string State::name() {
	return label;
}

CompositeState::CompositeState(std::string label) {
	this->label = label;
	subDiagram.parent = this;
}

CompositeState::~CompositeState() {
}

Diagram* CompositeState::getDiagram() {
	return &subDiagram;
}

void CompositeState::actionEntry() {
	subDiagram.reset();
}

void CompositeState::actionDo() {
	subDiagram.update();
}

#include <iostream>

std::string CompositeState::name() {
	//	std::cout << name() + ":" + subDiagram.getCurrentState()->name() <<
	//std::endl;
	return State::name() + ":" + subDiagram.getCurrentState()->name();
}
