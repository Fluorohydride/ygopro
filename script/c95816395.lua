--どぐう
function c95816395.initial_effect(c)
	--to grave
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_TO_GRAVE)
	e1:SetOperation(c95816395.regop)
	c:RegisterEffect(e1)
end
function c95816395.regop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if rp~=tp and c:IsReason(REASON_EFFECT) and not c:IsReason(REASON_RETURN) then
		local e1=Effect.CreateEffect(c)
		e1:SetDescription(aux.Stringid(95816395,0))
		e1:SetCategory(CATEGORY_TOHAND+CATEGORY_SEARCH)
		e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
		e1:SetCode(EVENT_PHASE+PHASE_END)
		e1:SetCountLimit(1)
		e1:SetRange(LOCATION_GRAVE)
		e1:SetTarget(c95816395.thtg)
		e1:SetOperation(c95816395.thop)
		e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END)
		c:RegisterEffect(e1)
	end
end
function c95816395.cfilter(c,lv)
	return c:GetLevel()==lv
end
function c95816395.filter(c,tp)
	return c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
		and not Duel.IsExistingMatchingCard(c95816395.cfilter,tp,LOCATION_MZONE+LOCATION_GRAVE,0,1,nil,c:GetLevel())
end
function c95816395.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.IsExistingMatchingCard(c95816395.filter,tp,LOCATION_DECK,0,1,nil,tp) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c95816395.thop(e,tp,eg,ep,ev,re,r,rp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
	local g=Duel.SelectMatchingCard(tp,c95816395.filter,tp,LOCATION_DECK,0,1,1,nil,tp)
	if g:GetCount()>0 then
		Duel.SendtoHand(g,nil,REASON_EFFECT)
		Duel.ConfirmCards(1-tp,g)
	end
end
