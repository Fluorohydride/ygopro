--霊廟の守護者
function c66752837.initial_effect(c)
	--double tribute
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_DOUBLE_TRIBUTE)
	e1:SetValue(c66752837.tricon)
	c:RegisterEffect(e1)
	--spsummon
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOHAND)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e2:SetCode(EVENT_TO_GRAVE)
	e2:SetRange(LOCATION_HAND+LOCATION_GRAVE)
	e2:SetProperty(EFFECT_FLAG_DAMAGE_STEP+EFFECT_FLAG_DELAY)
	e2:SetCountLimit(1,66752837)
	e2:SetCondition(c66752837.spcon)
	e2:SetTarget(c66752837.sptg)
	e2:SetOperation(c66752837.spop)
	c:RegisterEffect(e2)
end
function c66752837.tricon(e,c)
	return c:IsRace(RACE_DRAGON)
end
function c66752837.cfilter(c)
	return c:GetPreviousRaceOnField()==RACE_DRAGON and c:IsPreviousPosition(POS_FACEUP) and c:IsPreviousLocation(LOCATION_MZONE) and c:IsReason(REASON_EFFECT+REASON_BATTLE) and not c:IsCode(66752837)
end
function c66752837.cfilter2(c)
	return c66752837.cfilter(c) and c:IsType(TYPE_NORMAL)
end
function c66752837.spcon(e,tp,eg,ep,ev,re,r,rp)
	if eg:IsExists(c66752837.cfilter2,1,nil) then
		e:SetLabel(1)
		return true
	else
		return eg:IsExists(c66752837.cfilter,1,nil)
	end
end
function c66752837.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c66752837.thfilter(c)
	return c:IsRace(RACE_DRAGON) and c:IsType(TYPE_NORMAL) and c:IsAbleToHand() and not c:IsHasEffect(EFFECT_NECRO_VALLEY)
end
function c66752837.spop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) and Duel.SpecialSummon(c,0,tp,tp,false,false,POS_FACEUP)~=0 and e:GetLabel()==1
		and Duel.IsExistingMatchingCard(c66752837.thfilter,tp,LOCATION_GRAVE,0,1,nil) and Duel.SelectYesNo(tp,aux.Stringid(66752837,0)) then
		Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
		local g=Duel.SelectMatchingCard(tp,c66752837.thfilter,tp,LOCATION_GRAVE,0,1,1,nil)
		if g:GetCount()>0 then
			Duel.SendtoHand(g,nil,REASON_EFFECT)
			Duel.ConfirmCards(1-tp,g)
		end
	end
end
