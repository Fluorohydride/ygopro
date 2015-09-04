--ネクロの魔導書
function c52628687.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetCountLimit(1,52628687+EFFECT_COUNT_CODE_OATH)
	e1:SetCost(c52628687.cost)
	e1:SetTarget(c52628687.target)
	e1:SetOperation(c52628687.operation)
	c:RegisterEffect(e1)
end
function c52628687.cost(e,tp,eg,ep,ev,re,r,rp,chk)
	e:SetLabel(100)
	return true
end
function c52628687.cfilter(c,e,tp)
	return c:IsRace(RACE_SPELLCASTER) and c:GetLevel()>0 and c:IsAbleToRemoveAsCost()
		and Duel.IsExistingTarget(c52628687.spfilter,tp,LOCATION_GRAVE,0,1,c,e,tp)
end
function c52628687.cffilter(c)
	return c:IsSetCard(0x106e) and c:IsType(TYPE_SPELL) and not c:IsPublic()
end
function c52628687.spfilter(c,e,tp)
	return c:IsRace(RACE_SPELLCASTER) and c:GetLevel()>0 and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c52628687.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c52628687.spfilter(chkc,e,tp) end
	if chk==0 then
		if e:GetLabel()~=100 then return false end
		e:SetLabel(0)
		return Duel.GetLocationCount(tp,LOCATION_MZONE)>0
			and Duel.IsExistingMatchingCard(c52628687.cfilter,tp,LOCATION_GRAVE,0,1,nil,e,tp)
			and Duel.IsExistingMatchingCard(c52628687.cffilter,tp,LOCATION_HAND,0,1,e:GetHandler())
	end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local rg=Duel.SelectMatchingCard(tp,c52628687.cfilter,tp,LOCATION_GRAVE,0,1,1,nil,e,tp)
	e:SetLabel(rg:GetFirst():GetLevel())
	Duel.Remove(rg,POS_FACEUP,REASON_COST)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	local cg=Duel.SelectMatchingCard(tp,c52628687.cffilter,tp,LOCATION_HAND,0,1,1,e:GetHandler())
	Duel.ConfirmCards(1-tp,cg)
	Duel.ShuffleHand(tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectTarget(tp,c52628687.spfilter,tp,LOCATION_GRAVE,0,1,1,rg:GetFirst(),e,tp)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,e:GetHandler(),1,0,0)
end
function c52628687.eqlimit(e,c)
	return c==e:GetLabelObject()
end
function c52628687.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local tc=Duel.GetFirstTarget()
	if c:IsRelateToEffect(e) and tc:IsRelateToEffect(e)
		and Duel.SpecialSummonStep(tc,0,tp,tp,false,false,POS_FACEUP_ATTACK) then
		--levelup
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_EQUIP)
		e1:SetCode(EFFECT_UPDATE_LEVEL)
		e1:SetValue(e:GetLabel())
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
		Duel.Equip(tp,c,tc)
		Duel.SpecialSummonComplete()
		--Add Equip limit
		local e2=Effect.CreateEffect(c)
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_EQUIP_LIMIT)
		e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		e2:SetLabelObject(tc)
		e2:SetValue(c52628687.eqlimit)
		c:RegisterEffect(e2)
	end
end
