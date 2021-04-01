/**
 * @file run.h
 * @brief Utilities functions for event processing
 */
#ifndef CPPHOTS_RUN_H
#define CPPHOTS_RUN_H

#include <vector>
#include <string>
#include <utility>

#include "layer.h"
#include "network.h"
#include "classification.h"


namespace cpphots {

/**
 * @brief Generic event processing function
 * 
 * Process a sequence of events using a generic processor class.
 * 
 * A processor is somenthing with the following methods:
 * 
 *   - void reset()
 *   - Events process(const event&, bool)
 * 
 * @tparam P processor type
 * @param processor the processor
 * @param events sequence of events
 * @param skip_check if true consider all events as valid
 * @return events emitted by the processor
 */
template<typename P>
Events process(P& processor, const Events& events, bool skip_check = false) {

    processor.reset();

    Events ret;
    for (const auto& ev : events) {
        Events nev = processor.process(ev, skip_check);
        ret.insert(ret.end(), nev.begin(), nev.end());
    }

    return ret;

}

/**
 * @brief Generic event processing function
 * 
 * Process a sequence of events using a generic processor class.
 * 
 * A processor is somenthing with the following methods:
 * 
 *   - void reset()
 *   - Events process(const event&, bool)
 * 
 * @tparam P processor type
 * @param processor the processor
 * @param events sequences of events
 * @param skip_check if true consider all events as valid
 * @return corresponding sequences of events emitted by the processor
 */
template<typename P>
std::vector<Events> process(P& processor, const std::vector<Events>& events, bool skip_check = false) {

    std::vector<Events> ret;

    for (const auto& evts : events) {
        ret.push_back(process(processor, evts, skip_check));
    }

    return ret;

}


/**
 * @brief Initialize and train layers in a network
 * 
 * This function initializes the prototypes of each layer and then train the layer with the events,
 * in a layer-by-layer sequence.
 * 
 * The layers in the network must inherit from both Classifier and TimeSurfacePool.
 * 
 * @param network the newtork
 * @param training_events events
 * @param initializer a clustering initializer function
 * @param skip_check if true consider all events as valid
 */
void train(Network& network, Events training_events, const ClustererInitializerType& initializer, bool skip_check = false);

/**
 * @brief Initialize and train layers in a network
 * 
 * This function initializes the prototypes of each layer and then train the layer with the events,
 * in a layer-by-layer sequence.
 * 
 * The layers in the network must inherit from both Classifier and TimeSurfacePool.
 * 
 * @param network the newtork
 * @param training_events sequences of events
 * @param initializer a clustering initializer function
 * @param use_all if true use all sequence to initialize the prototypes (all sequences will used for training regardless)
 * @param skip_check if true consider all events as valid
 */
void train(Network& network, std::vector<Events> training_events, const ClustererInitializerType& initializer, bool use_all = true, bool skip_check = false);

}

#endif