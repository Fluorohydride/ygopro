--オーバーレイ・イーター
function c81816475.initial_effect(c)
	--
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(81816475,0))
	e1:SetType(EFFECT_TYPE_IGNITION)
	e1:SetRange(LOCATION_GRAVE)
	e1:SetCost(c81816475.cost)
	e1:SetTarget(c81816475.target)
	e1:SetOperation(c81816475.operation)
	c:RegisterEffect(e1)
end
function c81816475.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsAbleToRemoveAsCost() end
	Duel.Remove(e:GetHandler(),POS_FACEUP,REASON_COST)
end
function c81816475.filter(c)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
end
function c81816475.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetOverlayCount(tp,0,1)~=0
		and Duel.IsExistingMatchingCard(c81816475.filter,tp,LOCATION_MZONE,0,1,nil) end
end
function c81816475.operation(e,tp,eg,ep,ev,re,r,rp)
	local g1=Duel.GetOverlayGroup(tp,0,1)
	local g2=Duel.GetMatchingGroup(c81816475.filter,tp,LOCATION_MZONE,0,nil)
	if g1:GetCount()==0 or g2:GetCount()==0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(81816475,2))
	local mg=g1:Select(tp,1,1,nil)
	Duel.Hint(HINT_SELECTMSG,tp,aux.Stringid(81816475,1))
	local tc=g2:Select(tp,1,1,nil):GetFirst()
	local oc=mg:GetFirst():GetOverlayTarget()
	Duel.Overlay(tc,mg)
	Duel.RaiseSingleEvent(oc,EVENT_DETACH_MATERIAL,e,0,0,0,0)
end
