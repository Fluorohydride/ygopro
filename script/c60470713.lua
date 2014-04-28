--マドルチェ・チケット
function c60470713.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	c:RegisterEffect(e1)
	--search
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(60470713,0))
	e2:SetCategory(CATEGORY_TOHAND+CATEGORY_SPECIAL_SUMMON)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetRange(LOCATION_SZONE)
	e2:SetCountLimit(1,60470713)
	e2:SetCode(EVENT_TO_DECK)
	e2:SetCondition(c60470713.condition)
	e2:SetTarget(c60470713.target)
	e2:SetOperation(c60470713.operation)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EVENT_TO_HAND)
	c:RegisterEffect(e3)
end
function c60470713.cfilter(c,tp)
	return c:IsControler(tp) and c:GetPreviousControler()==tp
		and (c:IsPreviousLocation(LOCATION_GRAVE) or (c:IsPreviousLocation(LOCATION_ONFIELD) and c:IsPreviousPosition(POS_FACEUP)))
		and c:IsSetCard(0x71) and not c:IsLocation(LOCATION_EXTRA)
end
function c60470713.condition(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(r,REASON_EFFECT)~=0 and eg:IsExists(c60470713.cfilter,1,nil,tp)
end
function c60470713.target(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsRelateToEffect(e) end
	Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c60470713.mfilter(c)
	return c:IsFaceup() and c:IsSetCard(0x71) and c:IsRace(RACE_FAIRY)
end
function c60470713.filter1(c)
	return c:IsSetCard(0x71) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c60470713.filter2(c,e,tp)
	return c:IsSetCard(0x71) and c:IsType(TYPE_MONSTER)
		and (c:IsAbleToHand() or c:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP_ATTACK))
end
function c60470713.operation(e,tp,eg,ep,ev,re,r,rp)
	if not e:GetHandler():IsRelateToEffect(e) then return end
	local b=Duel.GetLocationCount(tp,LOCATION_MZONE)>0 and Duel.IsExistingMatchingCard(c60470713.mfilter,tp,LOCATION_MZONE,0,1,nil)
	if not b then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c60470713.filter1,tp,LOCATION_DECK,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SendtoHand(g,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,g)
		end
	else
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c60470713.filter2,tp,LOCATION_DECK,0,1,1,nil,e,tp)
		local tc=g:GetFirst()
		if tc then
			if not tc:IsAbleToHand() or (tc:IsCanBeSpecialSummoned(e,0,tp,false,false,POS_FACEUP_ATTACK)
				and Duel.SelectYesNo(tp,aux.Stringid(60470713,1))) then
				Duel.SpecialSummon(tc,0,tp,tp,false,false,POS_FACEUP_ATTACK)
			else
				Duel.SendtoHand(tc,nil,REASON_EFFECT)
				Duel.ConfirmCards(1-tp,tc)
			end
		end
	end
end
