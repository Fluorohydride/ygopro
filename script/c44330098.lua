--冥府の使者ゴーズ
function c44330098.initial_effect(c)
	--special summon
	local e1=Effect.CreateEffect(c)
	e1:SetDescription(aux.Stringid(44330098,0))
	e1:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
	e1:SetProperty(EFFECT_FLAG_DAMAGE_STEP)
	e1:SetRange(LOCATION_HAND)
	e1:SetCode(EVENT_DAMAGE)
	e1:SetCondition(c44330098.sumcon)
	e1:SetTarget(c44330098.sumtg)
	e1:SetOperation(c44330098.sumop)
	c:RegisterEffect(e1)
	--special summon success
	local e2=Effect.CreateEffect(c)
	e2:SetDescription(aux.Stringid(44330098,1))
	e2:SetCategory(CATEGORY_SPECIAL_SUMMON+CATEGORY_TOKEN)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetCondition(c44330098.sumcon2)
	e2:SetTarget(c44330098.sumtg2)
	e2:SetOperation(c44330098.sumop2)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(44330098,2))
	e3:SetCategory(CATEGORY_DAMAGE)
	e3:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
	e3:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetCondition(c44330098.sumcon3)
	e3:SetTarget(c44330098.sumtg3)
	e3:SetOperation(c44330098.sumop3)
	e3:SetLabelObject(e1)
	c:RegisterEffect(e3)
end
function c44330098.filter(c)
	return not c:IsStatus(STATUS_LEAVE_CONFIRMED)
end
function c44330098.sumcon(e,tp,eg,ep,ev,re,r,rp)
	return ep==tp and tp~=rp and not Duel.IsExistingMatchingCard(c44330098.filter,tp,LOCATION_ONFIELD,0,1,nil)
end
function c44330098.sumtg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>0 
		and e:GetHandler():IsCanBeSpecialSummoned(e,0,tp,false,false) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,e:GetHandler(),1,0,0)
end
function c44330098.sumop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:IsRelateToEffect(e) then
		local sumtype=1
		if bit.band(r,REASON_BATTLE)~=0 then sumtype=2 end
		if Duel.SpecialSummon(c,sumtype,tp,tp,false,false,POS_FACEUP)==0 then return end
		e:SetLabel(ev)
	end
end
function c44330098.sumcon2(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SPECIAL+2
end
function c44330098.sumtg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetOperationInfo(0,CATEGORY_TOKEN,nil,1,0,0)
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,1,0,0)
	Duel.RegisterFlagEffect(tp,EFFECT_SPSUM_EFFECT_ACTIVATED,RESET_PHASE+PHASE_END,EFFECT_FLAG_OATH,1)
end
function c44330098.sumop2(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	local c=e:GetHandler()
	local val=e:GetLabelObject():GetLabel()
	if not Duel.IsPlayerCanSpecialSummonMonster(tp,44330099,0,0x4011,-2,-2,7,RACE_FAIRY,ATTRIBUTE_LIGHT) then return end
	local token=Duel.CreateToken(tp,44330099)
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SET_ATTACK)
	e1:SetValue(val)
	e1:SetReset(RESET_EVENT+0xfe0000)
	token:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_SET_DEFENCE)
	e2:SetValue(val)
	e2:SetReset(RESET_EVENT+0xfe0000)
	token:RegisterEffect(e2)
	Duel.SpecialSummon(token,0,tp,tp,false,false,POS_FACEUP)
end
function c44330098.sumcon3(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_SPECIAL+1
end
function c44330098.sumtg3(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	local d=e:GetLabelObject():GetLabel()
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(d)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,d)
end
function c44330098.sumop3(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
