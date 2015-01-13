--コストダウン
function c23265313.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetCost(c23265313.cost)
	e1:SetOperation(c23265313.activate)
	c:RegisterEffect(e1)
end
function c23265313.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,e:GetHandler()) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD)
end

function c23265313.filter(c)
	return c:IsLevelAbove(2)
end

function c23265313.activate(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c23265313.filter,tp,LOCATION_HAND,0,nil)
	local tc=g:GetFirst()
	while tc do
		local e4=Effect.CreateEffect(e:GetHandler())
		e4:SetType(EFFECT_TYPE_SINGLE)
		e4:SetCode(EFFECT_UPDATE_LEVEL)
		e4:SetValue(-2)
		e4:SetTargetRange(LOCATION_MZONE+LOCATION_HAND,0)
		e4:SetReset(RESET_PHASE+PHASE_END)
		tc:RegisterEffect(e4)
		tc=g:GetNext()
	end
end
