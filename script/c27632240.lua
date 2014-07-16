--虚空の騎士
function c27632240.initial_effect(c)
	--send to grave
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(27632240,0))
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e1:SetRange(LOCATION_MZONE)
	e1:SetCountLimit(1,EFFECT_COUNT_CODE_SINGLE)
	e1:SetCode(EVENT_TO_HAND)
	e1:SetCondition(c27632240.tgcon1)
	e1:SetTarget(c27632240.tgtg)
	e1:SetOperation(c27632240.tgop)
	c:RegisterEffect(e1)
	local e2=e1:Clone()
	e2:SetCode(EVENT_TO_DECK)
	e2:SetCondition(c27632240.tgcon2)
	c:RegisterEffect(e2)
end
function c27632240.cfilter1(c)
	return c:IsPreviousLocation(LOCATION_MZONE)
end
function c27632240.tgcon1(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c27632240.cfilter1,1,nil)
end
function c27632240.cfilter2(c)
	return c:IsPreviousLocation(LOCATION_MZONE) and c:IsLocation(LOCATION_DECK)
end
function c27632240.tgcon2(e,tp,eg,ep,ev,re,r,rp)
	return eg:IsExists(c27632240.cfilter2,1,nil)
end
function c27632240.tgtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c27632240.tgfilter(c)
	return c:IsType(TYPE_MONSTER) and c:IsAttribute(ATTRIBUTE_WIND) and c:IsAbleToGrave()
end
function c27632240.tgop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsFacedown() or not c:IsRelateToEffect(e) then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c27632240.tgfilter,tp,LOCATION_DECK,0,1,1,nil)
	if g:GetCount()>0 then
		Duel.SendtoGrave(g,REASON_EFFECT)
	end
end
