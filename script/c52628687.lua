--ネクロの魔導書
function c52628687.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetTarget(c52628687.target)
	e1:SetOperation(c52628687.operation)
	c:RegisterEffect(e1)
end
function c52628687.cfilter(c)
	return c:IsRace(RACE_SPELLCASTER) and not c:IsType(TYPE_XYZ) and c:IsAbleToRemoveAsCost()
end
function c52628687.cffilter(c)
	return c:IsSetCard(0x106e) and c:IsType(TYPE_SPELL) and not c:IsPublic()
end
function c52628687.spfilter(c,e,tp)
	return c:IsRace(RACE_SPELLCASTER) and not c:IsType(TYPE_XYZ) and c:IsCanBeSpecialSummoned(e,0,tp,false,false)
end
function c52628687.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_GRAVE) and chkc:IsControler(tp) and c52628687.spfilter(chkc,e,tp) end
	if chk==0 then
		if Duel.GetFlagEffect(tp,52628687)~=0 or Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return false end
		if not Duel.IsExistingMatchingCard(c52628687.cffilter,tp,LOCATION_HAND,0,1,e:GetHandler()) then return false end
		local rg=Duel.GetMatchingGroup(c52628687.cfilter,tp,LOCATION_GRAVE,0,nil)
		local spg=Duel.GetMatchingGroup(c52628687.spfilter,tp,LOCATION_GRAVE,0,nil,e,tp)
		return rg:GetCount()>0 and spg:GetCount()>0 and (rg:GetCount()>1 or spg:GetCount()>1 or rg:GetFirst()~=spg:GetFirst())
	end
	local rg=Duel.GetMatchingGroup(c52628687.cfilter,tp,LOCATION_GRAVE,0,nil)
	local spg=Duel.GetMatchingGroup(c52628687.spfilter,tp,LOCATION_GRAVE,0,nil,e,tp)
	if spg:GetCount()==1 then rg:Sub(spg) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_REMOVE)
	local rm=rg:Select(tp,1,1,nil)
	e:SetLabel(rm:GetFirst():GetLevel())
	Duel.Remove(rm,POS_FACEUP,REASON_COST)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_CONFIRM)
	Duel.ConfirmCards(1-tp,Duel.SelectMatchingCard(tp,c52628687.cffilter,tp,LOCATION_HAND,0,1,1,e:GetHandler()))
	Duel.ShuffleHand(tp)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=spg:Select(tp,1,1,rm:GetFirst())
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,g,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_EQUIP,e:GetHandler(),1,0,0)
	Duel.RegisterFlagEffect(tp,52628687,RESET_PHASE+PHASE_END,EFFECT_FLAG_OATH,1)
end
function c52628687.eqlimit(e,c)
	return e:GetOwner()==c
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
		Duel.SpecialSummonComplete()
		Duel.Equip(tp,c,tc)
		--Add Equip limit
		local e2=Effect.CreateEffect(tc)
		e2:SetType(EFFECT_TYPE_SINGLE)
		e2:SetCode(EFFECT_EQUIP_LIMIT)
		e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e2:SetReset(RESET_EVENT+0x1fe0000)
		e2:SetValue(c52628687.eqlimit)
		c:RegisterEffect(e2)
	end
end
