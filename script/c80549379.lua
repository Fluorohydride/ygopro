--ドラグニティ－ジャベリン
function c80549379.initial_effect(c)
	--send replace
	local e1=Effect.CreateEffect(c)
	e1:SetCode(EFFECT_SEND_REPLACE)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetProperty(EFFECT_FLAG_SINGLE_RANGE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTarget(c80549379.reptg)
	e1:SetOperation(c80549379.repop)
	c:RegisterEffect(e1)
end
function c80549379.filter(c)
	return c:IsFaceup() and c:IsSetCard(0x29) and c:IsRace(RACE_WINDBEAST)
end
function c80549379.reptg(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	if chk==0 then return c:IsFaceup() and c:GetDestination()==LOCATION_GRAVE and c:IsReason(REASON_DESTROY) end
	if Duel.GetLocationCount(tp,LOCATION_SZONE)<=0
		or not Duel.IsExistingMatchingCard(c80549379.filter,tp,LOCATION_MZONE,0,1,e:GetHandler()) then return false end
	return Duel.SelectEffectYesNo(tp,c)
end
function c80549379.repop(e,tp,eg,ep,ev,re,r,rp,chk)
	local c=e:GetHandler()
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_EQUIP)
	local g=Duel.SelectMatchingCard(tp,c80549379.filter,tp,LOCATION_MZONE,0,1,1,e:GetHandler())
	local tc=g:GetFirst()
	if Duel.Equip(tp,c,tc,false) then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_EQUIP_LIMIT)
		e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		e1:SetValue(c80549379.eqlimit)
		e1:SetLabelObject(tc)
		c:RegisterEffect(e1)
	end
end
function c80549379.eqlimit(e,c)
	return c==e:GetLabelObject()
end
