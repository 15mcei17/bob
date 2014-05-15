/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Fri 17 May 2013 14:34:17 CEST
 *
 * @brief A registration system for new activation routines
 *
 * Copyright (C) Idiap Research Institute, Martigny, Switzerland
 */

#ifndef BOB_MACHINE_ACTIVATIONREGISTRY_H
#define BOB_MACHINE_ACTIVATIONREGISTRY_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "bob/machine/Activation.h"

namespace bob { namespace machine {

  /**
   * The ActivationRegistry holds registered loaders for different types of
   * Activation functions. 
   */
  class ActivationRegistry {

    public: //static access
      
      /**
       * Returns the singleton
       */
      static boost::shared_ptr<ActivationRegistry> instance();

      static const std::map<std::string, activation_factory_t>& getFactories ()
      {
        boost::shared_ptr<ActivationRegistry> ptr = instance();
        return ptr->s_id2factory;
      }
 
    public: //object access

      void registerActivation(const std::string& unique_identifier,
          activation_factory_t factory);

      void deregisterFactory(const std::string& unique_identifier);

      activation_factory_t find(const std::string& unique_identifier);

      bool isRegistered(const std::string& unique_identifier);

    private:

      ActivationRegistry (): s_id2factory() {}

      // Not implemented
      ActivationRegistry (const ActivationRegistry&);

      std::map<std::string, activation_factory_t> s_id2factory;
    
  };

}}

#endif /* BOB_MACHINE_ACTIVATIONREGISTRY_H */

