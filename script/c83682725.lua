--テールスイング
function c83682725.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_TOHAND)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetTarget(c83682725.target)
	e1:SetOperation(c83682725.activate)
	c:RegisterEffect(e1)
end
function c83682725.filter(c,tp)
	return c:IsFaceup() and c:IsRace(RACE_DINOSAUR) and c:IsLevelAbove(5)
		and Duel.IsExistingMatchingCard(c83682725.dfilter,tp,0,LOCATION_MZONE,1,nil,c:GetLevel())
end
function c83682725.dfilter(c,lv)
	return (c:IsFacedown() or c:IsLevelBelow(lv-1)) and c:IsAbleToHand()
end
function c83682725.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c83682725.filter(chkc,tp) end
	if chk==0 then return Duel.IsExistingTarget(c83682725.filter,tp,LOCATION_MZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	local g=Duel.SelectTarget(tp,c83682725.filter,tp,LOCATION_MZONE,0,1,1,nil,tp)
	local sg=Duel.GetMatchingGroup(c83682725.dfilter,tp,0,LOCATION_MZONE,nil,g:GetFirst():GetLevel())
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,sg,1,0,0)
end
function c83682725.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if tc:IsFaceup() and tc:IsRelateToEffect(e) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RTOHAND)
		local sg=Duel.SelectMatchingCard(tp,c83682725.dfilter,tp,0,LOCATION_MZONE,1,2,nil,tc:GetLevel())
		Duel.SendtoHand(sg,nil,REASON_EFFECT)
	end
end
