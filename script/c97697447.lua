--アビストローム
function c97697447.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c97697447.cost)
	e1:SetTarget(c97697447.target)
	e1:SetOperation(c97697447.activate)
	c:RegisterEffect(e1)
end
function c97697447.cfilter(c)
	return c:IsFaceup() and c:IsCode(22702055)
end
function c97697447.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(1)
	local g=Duel.GetMatchingGroup(c97697447.cfilter,tp,LOCATION_ONFIELD,0,nil)
	if chk==0 then return g:GetCount()>0 and g:FilterCount(Card.IsAbleToGraveAsCost,nil)==g:GetCount() end
	Duel.SendtoGrave(g,REASON_COST)
end
function c97697447.filter(c,tp)
	return (c:IsFacedown() or c:IsControler(1-tp) or c:GetCode()~=22702055) and c:IsType(TYPE_SPELL+TYPE_TRAP)
end
function c97697447.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then
		if e:GetLabel()==0 then
			return Duel.IsExistingMatchingCard(Card.IsType,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler(),TYPE_SPELL+TYPE_TRAP)
		end
		e:SetLabel(0)
		return Duel.IsExistingMatchingCard(c97697447.filter,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,1,e:GetHandler(),tp)
	end
	e:SetLabel(0)
	local g=Duel.GetMatchingGroup(Card.IsType,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,e:GetHandler(),TYPE_SPELL+TYPE_TRAP)
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,g,g:GetCount(),0,0)
end
function c97697447.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(Card.IsType,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,e:GetHandler(),TYPE_SPELL+TYPE_TRAP)
	Duel.SendtoGrave(g,REASON_EFFECT)
end
