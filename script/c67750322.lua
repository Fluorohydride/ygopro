--スカル・マイスター
function c67750322.initial_effect(c)
	--Inactivate
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(67750322,0))
	e1:SetCategory(CATEGORY_DISABLE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_QUICK_O)
	e1:SetCode(EVENT_CHAINING)
	e1:SetRange(LOCATION_HAND)
	e1:SetCondition(c67750322.discon)
	e1:SetCost(c67750322.discost)
	e1:SetTarget(c67750322.distg)
	e1:SetOperation(c67750322.disop)
	c:RegisterEffect(e1)
end
function c67750322.discon(e,tp,eg,ep,ev,re,r,rp)
	local loc=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	return ep~=tp and Duel.IsChainNegatable(ev) and loc==LOCATION_GRAVE
end
function c67750322.discost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToGraveAsCost() end
	Duel.SendtoGrave(e:GetHandler(),REASON_COST)
end
function c67750322.distg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DISABLE,eg,1,0,0)
end
function c67750322.disop(e,tp,eg,ep,ev,re,r,rp)
	Duel.NegateEffect(ev)
end
