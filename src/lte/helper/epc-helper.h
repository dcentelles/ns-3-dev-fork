/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2011 Centre Tecnologic de Telecomunicacions de Catalunya (CTTC)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Jaume Nin <jnin@cttc.es>
 *         Nicola Baldo <nbaldo@cttc.es>
 */

#ifndef EPC_HELPER_H
#define EPC_HELPER_H

#include <ns3/object.h>
#include <ns3/ipv4-address-helper.h>
#include <ns3/data-rate.h>
#include <ns3/epc-tft.h>


namespace ns3 {

class Node;
class NetDevice;
class VirtualNetDevice;
class EpcSgwPgwApplication;
class EpcX2;

/**
 * \brief Helper class to handle the creation of the EPC entities and protocols.
 *
 * This Helper will create an EPC network topology comprising of a
 * single node that implements both the SGW and PGW functionality, and
 * is connected to all the eNBs in the simulation by means of the S1-U
 * interface. 
 */
class EpcHelper : public Object
{
public:
  
  /** 
   * Constructor
   */
  EpcHelper ();

  /** 
   * Destructor
   */  
  virtual ~EpcHelper ();
  
  // inherited from Object
  static TypeId GetTypeId (void);
  virtual void DoDispose ();

  
  /** 
   * Add an eNB to the EPC
   * 
   * \param enbNode the previosuly created eNB node which is to be
   * added to the EPC
   * \param lteEnbNetDevice the LteEnbNetDevice of the eNB node
   */
  void AddEnb (Ptr<Node> enbNode, Ptr<NetDevice> lteEnbNetDevice);


  /** 
   * Add an X2 interface between two eNB
   * 
   * \param enbNode1 one eNB peer of the X2 interface
   * \param enbNode2 the other eNB peer of the X2 interface
   */
  void AddX2Interface (Ptr<Node> enbNode1, Ptr<Node> enbNode2);

  void SendHandoverRequest (Ptr<Node> ueNode, Ptr<Node> sourceEnbNode, Ptr<Node> targetEnbNode);


  /** 
   * Activate an EPS bearer, setting up the corresponding S1-U tunnel.
   * 
   * 
   * 
   * \param ueLteDevice the Ipv4-enabled device of the UE, normally connected via the LTE radio interface
   * \param enbLteDevice the non-Ipv4-enabled device of the eNB
   * \param tft the Traffic Flow Template of the new bearer
   * \param rnti the Radio Network Temporary Identifier that identifies the UE
   * \param lcid the Logical Channel IDentifier of the corresponding RadioBearer
   */
  void ActivateEpsBearer (Ptr<NetDevice> ueLteDevice, Ptr<NetDevice> enbLteDevice, Ptr<EpcTft> tft, uint16_t rnti, uint8_t lcid);


  /** 
   * 
   * \return a pointer to the node implementing PGW
   * functionality. Note that in this particular implementation this
   * node will also hold the SGW functionality. The primary use
   * intended for this method is to allow the user to configure the Gi
   * interface of the PGW, i.e., to connect the PGW to the internet.
   */
  Ptr<Node> GetPgwNode ();

  /** 
   * Assign IPv4 addresses to UE devices
   * 
   * \param ueDevices the set of UE devices
   * 
   * \return the interface container, \see Ipv4AddressHelper::Assign() which has similar semantics
   */
  Ipv4InterfaceContainer AssignUeIpv4Address (NetDeviceContainer ueDevices);


  /** 
   * 
   * \return the address of the Default Gateway to be used by UEs to reach the internet
   */
  Ipv4Address GetUeDefaultGatewayAddress ();



private:

  /**
   * SGW-PGW network element
   */

  /** 
   * helper to assign addresses to UE devices as well as to the TUN device of the SGW/PGW
   */
  Ipv4AddressHelper m_ueAddressHelper; 
  
  Ptr<Node> m_sgwPgw; 
  Ptr<EpcSgwPgwApplication> m_sgwPgwApp;
  Ptr<VirtualNetDevice> m_tunDevice;
  

  /**
   * S1-U interfaces
   */

  /** 
   * helper to assign addresses to S1-U NetDevices 
   */
  Ipv4AddressHelper m_s1uIpv4AddressHelper; 

  DataRate m_s1uLinkDataRate;
  Time     m_s1uLinkDelay;
  uint16_t m_s1uLinkMtu;

  /**
   * UDP port where the GTP-U Socket is bound, fixed by the standard as 2152
   */
  uint16_t m_gtpuUdpPort;

  
  /**
   * X2 interfaces
   * TODO To separate between X2-U interfaces and X2-C interfaces
   */

  /** 
   * helper to assign addresses to X2 NetDevices 
   */
  Ipv4AddressHelper m_x2Ipv4AddressHelper; 

  DataRate m_x2LinkDataRate;
  Time     m_x2LinkDelay;
  uint16_t m_x2LinkMtu;


  /**
   * UDP port where the GTP-U Socket is bound, fixed by the standard as 2152 TODO Check value in the spec
   */
  uint16_t m_x2cUdpPort;


  /**
   * X2 Applications
   */
  class EpcX2NodePeers : public SimpleRefCount<EpcX2NodePeers>
  {
  public:
    EpcX2NodePeers (Ptr<Node> enbPeer1, Ptr<Node> enbPeer2);
    virtual ~EpcX2NodePeers (void);

    bool operator< (const EpcX2NodePeers &) const;

    Ptr<Node> m_enbPeer1;
    Ptr<Node> m_enbPeer2;
  };

  class EpcX2ApplicationPairs : public SimpleRefCount<EpcX2ApplicationPairs>
  {
  public:
    EpcX2ApplicationPairs ();
    EpcX2ApplicationPairs (Ptr<EpcX2> m_x2AppPair1, Ptr<EpcX2> m_x2AppPair2);
    virtual ~EpcX2ApplicationPairs (void);

    EpcX2ApplicationPairs& operator= (const EpcX2ApplicationPairs &);

    Ptr<EpcX2> m_x2AppPair1;
    Ptr<EpcX2> m_x2AppPair2;
  };

  std::map < EpcX2NodePeers, EpcX2ApplicationPairs > m_x2Interfaces;

};




} // namespace ns3

#endif // EPC_HELPER_H
