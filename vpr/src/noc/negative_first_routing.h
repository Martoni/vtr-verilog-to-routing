#ifndef VTR_NEGATIVE_FIRST_ROUTING_H
#define VTR_NEGATIVE_FIRST_ROUTING_H

#include "turn_model_routing.h"

class NegativeFirstRouting : public TurnModelRouting {
  public:
    NegativeFirstRouting(const NocStorage& noc_model,
                         const std::optional<std::reference_wrapper<const NocVirtualBlockStorage>>& noc_virtual_blocks);
    ~NegativeFirstRouting() override;

  private:
    const std::vector<TurnModelRouting::Direction>& get_legal_directions(NocRouterId src_router_id,
                                                                         NocRouterId curr_router_id,
                                                                         NocRouterId dst_router_id) override;

    TurnModelRouting::Direction select_next_direction(const std::vector<TurnModelRouting::Direction>& legal_directions,
                                                      NocRouterId src_router_id,
                                                      NocRouterId dst_router_id,
                                                      NocRouterId curr_router_id,
                                                      NocTrafficFlowId traffic_flow_id) override;
};

#endif //VTR_NEGATIVE_FIRST_ROUTING_H