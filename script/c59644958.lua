--オーバーレイ・オウル
function c59644958.initial_effect(c)
	--detach
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(59644958,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1)
	e1:SetCost(c59644958.cost)
	e1:SetTarget(c59644958.target)
	e1:SetOperation(c59644958.operation)
	c:RegisterEffect(e1)
end
function c59644958.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckLPCost(tp,600) end
	Duel.PayLPCost(tp,600)
end
function c59644958.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.CheckRemoveOverlayCard(tp,1,1,1,REASON_EFFECT) end
end
function c59644958.operation(e,tp,eg,ep,ev,re,r,rp)
	local sg=Duel.GetMatchingGroup(Card.CheckRemoveOverlayCard,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp,1,REASON_EFFECT)
	if sg:GetCount()==0 then return end
	if sg:GetCount()>1 then
		Duel.Hint(HINT_SELECTMSG,tp,532)
		sg=sg:Select(tp,1,1,nil)
		Duel.HintSelection(sg)
	end
	sg:GetFirst():RemoveOverlayCard(tp,1,1,REASON_EFFECT)
end
