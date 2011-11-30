--ラヴァル炎火山の侍女
function c2407147.initial_effect(c)
	--to grave
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(2407147,0))
	e1:SetCategory(CATEGORY_TOGRAVE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetCondition(c2407147.condition)
	e1:SetTarget(c2407147.target)
	e1:SetOperation(c2407147.operation)
	c:RegisterEffect(e1)
end
function c2407147.cfilter(c)
	return c:IsSetCard(0x39) and c:GetCode()~=2407147
end
function c2407147.condition(e,tp,eg,ep,ev,re,r,rp)
	return Duel.IsExistingMatchingCard(c2407147.cfilter,tp,LOCATION_GRAVE,0,1,nil) and not e:GetHandler():IsReason(REASON_RETURN)
end
function c2407147.filter(c)
	return c:IsSetCard(0x39) and c:IsAbleToGrave()
end
function c2407147.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c2407147.filter,tp,LOCATION_DECK,0,1,nil) end
	Duel.SetOperationInfo(0,CATEGORY_TOGRAVE,nil,1,tp,LOCATION_DECK)
end
function c2407147.operation(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TOGRAVE)
	local g=Duel.SelectMatchingCard(tp,c2407147.filter,tp,LOCATION_DECK,0,1,1,nil)
	Duel.SendtoGrave(g,REASON_EFFECT)
end
