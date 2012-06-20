--クリオスフィンクス
function c18654201.initial_effect(c)
	--reg
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetOperation(c18654201.regop)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(18654201,0))
	e2:SetCategory(CATEGORY_TOGRAVE)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(18654201)
	e2:SetTarget(c18654201.hdtg)
	e2:SetOperation(c18654201.hdop)
	c:RegisterEffect(e2)
end
function c18654201.filter(c,tp)
	return c:IsControler(tp) and c:IsPreviousLocation(LOCATION_MZONE)
end
function c18654201.regop(e,tp,eg,ep,ev,re,r,rp)
	local p1=false local p2=false
	if eg:IsExists(c18654201.filter,1,nil,0) then p1=true end
	if eg:IsExists(c18654201.filter,1,nil,1) then p2=true end
	local c=e:GetHandler()
	if p1 and p2 then
		Duel.RaiseSingleEvent(c,18654201,re,r,rp,PLAYER_ALL,0)
	elseif p1 then
		Duel.RaiseSingleEvent(c,18654201,re,r,rp,0,0)
	elseif p2 then
		Duel.RaiseSingleEvent(c,18654201,re,r,rp,1,0)
	end
end
function c18654201.hdtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,ep,LOCATION_HAND)
end
function c18654201.hdop(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	if ep==PLAYER_ALL then
		Duel.DiscardHand(0,nil,1,1,REASON_EFFECT)
		Duel.DiscardHand(1,nil,1,1,REASON_EFFECT)
	else
		Duel.DiscardHand(ep,nil,1,1,REASON_EFFECT)
	end
end
