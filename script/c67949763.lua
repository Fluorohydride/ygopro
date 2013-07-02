--罪鍵の法－シン・キー・ロウ
function c67949763.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetProperty(EFFECT_FLAG_CARD_TARGET)
	e1:SetHintTiming(0,TIMING_END_PHASE)
	e1:SetTarget(c67949763.target)
	e1:SetOperation(c67949763.activate)
	c:RegisterEffect(e1)
end
function c67949763.filter(c,tp)
	return c:IsFaceup() and c:IsType(TYPE_XYZ)
		and Duel.IsPlayerCanSpecialSummonMonster(tp,67949764,0x87,0x4011,-2,0,1,RACE_FIEND,ATTRIBUTE_DARK)
end
function c67949763.target(e,tp,eg,ep,ev,re,r,rp,chk,chkc)
	if chkc then return chkc:IsLocation(LOCATION_MZONE) and chkc:IsControler(tp) and c67949763.filter(chkc,tp) end
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>2
		and Duel.IsExistingTarget(c67949763.filter,tp,LOCATION_MZONE,0,1,nil,tp) end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_TARGET)
	Duel.SelectTarget(tp,c67949763.filter,tp,LOCATION_MZONE,0,1,1,nil,tp)
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,3,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,3,0,0)
end
function c67949763.activate(e,tp,eg,ep,ev,re,r,rp)
	local tc=Duel.GetFirstTarget()
	if Duel.GetLocationCount(tp,LOCATION_MZONE)>2 then
		local rfid=tc:GetRealFieldID()
		local atk=0
		local cr=false
		if tc:IsFaceup() and tc:IsRelateToEffect(e) then
			atk=tc:GetAttack()
			cr=true
		end
		if not Duel.IsPlayerCanSpecialSummonMonster(tp,67949764,0x87,0x4011,-2,0,1,RACE_FIEND,ATTRIBUTE_DARK) then return end
		if cr then
			local de=Effect.CreateEffect(e:GetHandler())
			de:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
			de:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
			de:SetCode(EVENT_LEAVE_FIELD)
			de:SetOperation(c67949763.desop)
			de:SetLabel(rfid)
			tc:RegisterEffect(de,true)
		end
		for i=1,3 do
			local token=Duel.CreateToken(tp,67949764)
			Duel.SpecialSummonStep(token,0,tp,tp,false,false,POS_FACEUP)
			local e1=Effect.CreateEffect(e:GetHandler())
			e1:SetType(EFFECT_TYPE_SINGLE)
			e1:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
			e1:SetReset(RESET_EVENT+0x1fe0000)
			token:RegisterEffect(e1,true)
			local e2=Effect.CreateEffect(e:GetHandler())
			e2:SetType(EFFECT_TYPE_SINGLE)
			e2:SetCode(EFFECT_UNRELEASABLE_NONSUM)
			e2:SetValue(1)
			e2:SetReset(RESET_EVENT+0x1fe0000)
			token:RegisterEffect(e2,true)
			local e3=Effect.CreateEffect(e:GetHandler())
			e3:SetType(EFFECT_TYPE_SINGLE)
			e3:SetCode(EFFECT_SET_ATTACK)
			e3:SetValue(atk)
			e3:SetLabelObject(tc)
			e3:SetReset(RESET_EVENT+0x1fe0000)
			token:RegisterEffect(e3,true)
			if cr then
				token:RegisterFlagEffect(67949764,RESET_EVENT+0x1fe0000,0,0,rfid)
				tc:CreateRelation(token,RESET_EVENT+0x1fc0000)
			end
		end
		Duel.SpecialSummonComplete()
	end
end
function c67949763.desfilter(c,rfid)
	return c:GetFlagEffectLabel(67949764)==rfid
end
function c67949763.desop(e,tp,eg,ep,ev,re,r,rp)
	local g=Duel.GetMatchingGroup(c67949763.desfilter,tp,LOCATION_MZONE,0,nil,e:GetLabel())
	Duel.Destroy(g,REASON_EFFECT)
	e:Reset()
end
