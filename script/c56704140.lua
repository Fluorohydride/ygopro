--氷結界の風水師
function c56704140.initial_effect(c)
	--untargetable
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(56704140,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_NO_TURN_RESET)
	e1:SetCountLimit(1)
	e1:SetCost(c56704140.cost)
	e1:SetTarget(c56704140.target)
	e1:SetOperation(c56704140.operation)
	c:RegisterEffect(e1)
end
function c56704140.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(Card.IsDiscardable,tp,LOCATION_HAND,0,1,nil) end
	Duel.DiscardHand(tp,Card.IsDiscardable,1,1,REASON_COST+REASON_DISCARD)
end
function c56704140.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.Hint(HINT_SELECTMSG,tp,0)
	local aat=Duel.AnnounceAttribute(tp,1,0x7f)
	e:SetLabel(aat)
end
function c56704140.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFacedown() or not c:IsRelateToEffect(e) then return end
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e1:SetLabel(e:GetLabel())
	e1:SetValue(c56704140.tgval)
	e1:SetReset(RESET_EVENT+0x1ff0000)
	c:RegisterEffect(e1)
end
function c56704140.tgval(e,c)
	return c:IsAttribute(e:GetLabel()) and not c:IsImmuneToEffect(e)
end
