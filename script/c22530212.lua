--マジック・ハンド
function c22530212.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetCategory(CATEGORY_TOGRAVE+CATEGORY_DAMAGE)
	e1:SetProperty(EFFECT_FLAG_NO_TURN_RESET)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetCountLimit(1)
	e1:SetCondition(c22530212.condition)
	e1:SetTarget(c22530212.target)
	e1:SetOperation(c22530212.activate)
	c:RegisterEffect(e1)
end
function c22530212.cfilter(c,tp)
	return c:IsControler(tp) and c:IsPreviousLocation(LOCATION_DECK) and not c:IsReason(REASON_DRAW)
end
function c22530212.condition(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c22530212.cfilter,1,nil,1-tp)
end
function c22530212.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,800)
end
function c22530212.filter(c,e,tp)
	return c:IsRelateToEffect(e) and c:IsControler(tp) and c:IsPreviousLocation(LOCATION_DECK)
end
function c22530212.activate(e,tp,eg,ep,ev,re,r,rp)
	local sg=eg:Filter(c22530212.filter,nil,e,1-tp)
	if sg:GetCount()==0 then
		Duel.SendtoGrave(sg,REASON_EFFECT) 
		if sg:IsExists(Card.IsLocation,1,nil,LOCATION_GRAVE) then
			Duel.Damage(1-tp,800,REASON_EFFECT)
		end
	end
end
