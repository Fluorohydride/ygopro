--セイバー・リフレクト
function c35037880.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_RECOVER+CATEGORY_DAMAGE+CATEGORY_TOHAND+CATEGORY_SEARCH)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_DAMAGE)
	e1:SetCountLimit(1,35037880+EFFECT_COUNT_CODE_OATH)
	e1:SetCondition(c35037880.condition)
	e1:SetTarget(c35037880.target)
	e1:SetOperation(c35037880.activate)
	c:RegisterEffect(e1)
end
function c35037880.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x100d)
end
function c35037880.condition(e,tp,eg,ep,ev,re,r,rp)
	return ep==tp and Duel.IsExistingMatchingCard(c35037880.filter,tp,LOCATION_MZONE,0,1,nil)
end
function c35037880.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_RECOVER,nil,0,tp,ev)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,ev)
end
function c35037880.thfilter(c)
	return (c:IsSetCard(0xd) and c:IsType(TYPE_SPELL+TYPE_TRAP) or c:IsSetCard(0xb0)) and c:IsAbleToHand()
end
function c35037880.activate(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local ct=Duel.Recover(tp,ev,REASON_EFFECT)
	local ct1=Duel.Damage(1-tp,ev,REASON_EFFECT)

	if ct+ct1 >0 and Duel.IsExistingMatchingCard(c35037880.thfilter,tp,LOCATION_DECK,0,1,nil)
		and Duel.SelectYesNo(tp,aux.Stringid(35037880,0))
	then 
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c35037880.thfilter,tp,LOCATION_DECK,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SendtoHand(g,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,g)
		end
	end
end