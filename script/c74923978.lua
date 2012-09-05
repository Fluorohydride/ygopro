--強制接収
function c74923978.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_DISCARD)
	e1:SetCondition(c74923978.condition)
	c:RegisterEffect(e1)
	--handes
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(74923978,0))
	e2:SetCategory(CATEGORY_HANDES)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCode(EVENT_DISCARD)
	e2:SetCondition(c74923978.condition)
	e2:SetTarget(c74923978.target)
	e2:SetOperation(c74923978.operation)
	c:RegisterEffect(e2)
end
function c74923978.cfilter(c,tp)
	return c:GetPreviousControler()==tp
end
function c74923978.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c74923978.cfilter,1,nil,tp)
end
function c74923978.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) end
	local ct=eg:FilterCount(c74923978.cfilter,nil,tp)
	e:SetLabel(ct)
	Duel.SetOperationInfo(0,CATEGORY_HANDES,nil,0,1-tp,ct)
end
function c74923978.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local ct=e:GetLabel()
	Duel.DiscardHand(1-tp,nil,ct,ct,REASON_EFFECT+REASON_DISCARD)
end
