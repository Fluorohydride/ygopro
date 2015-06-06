--ラーの翼神竜－球体型
function c10000050.initial_effect(c)
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--summon with 3 tribute
	local e2=Effect.CreateEffect(c)
	e2:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_LIMIT_SUMMON_PROC)
	e2:SetCondition(c10000050.ttcon)
	e2:SetOperation(c10000050.ttop)
	e2:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e2)
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_LIMIT_SET_PROC)
	e3:SetCondition(c10000050.setcon)
	c:RegisterEffect(e3)
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e4:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e4:SetCode(EVENT_SUMMON_SUCCESS)
	e4:SetOperation(c10000050.retreg)
	c:RegisterEffect(e4)
	--cannot attack
	local e5=Effect.CreateEffect(c)
	e5:SetType(EFFECT_TYPE_SINGLE)
	e5:SetCode(EFFECT_CANNOT_ATTACK)
	c:RegisterEffect(e5)
	--cannot be target
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCode(EFFECT_CANNOT_BE_BATTLE_TARGET)
	e6:SetValue(aux.imval1)
	c:RegisterEffect(e6)
	local e7=e6:Clone()
	e7:SetCode(EFFECT_CANNOT_BE_EFFECT_TARGET)
	e7:SetValue(aux.tgoval)
	c:RegisterEffect(e7)
	--special summon
	local e8=Effect.CreateEffect(c)
	e8:SetDescription(aux.Stringid(10000050,2))
	e8:SetCategory(CATEGORY_SPECIAL_SUMMON)
	e8:SetType(EFFECT_TYPE_IGNITION)
	e8:SetRange(LOCATION_MZONE)
	e8:SetCost(c10000050.spcost)
	e8:SetTarget(c10000050.sptg)
	e8:SetOperation(c10000050.spop)
	c:RegisterEffect(e8)
	local e9=Effect.CreateEffect(c)
	e9:SetType(EFFECT_TYPE_FIELD)
	e9:SetCode(EFFECT_EXTRA_RELEASE)
	e9:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_IGNORE_IMMUNE+EFFECT_FLAG_SET_AVAILABLE)
	e9:SetRange(LOCATION_HAND)
	e9:SetTargetRange(0,LOCATION_MZONE)
	e9:SetCondition(c10000050.check)
	c:RegisterEffect(e9)
end
function c10000050.check(e)
	return e:GetHandler():GetFlagEffect(10000050)~=0
end
function c10000050.ttcon(e,c)
	if c==nil then return true end
	local b1=Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>-3 and Duel.GetTributeCount(c)>=3
	c:RegisterFlagEffect(10000050,0,0,1)
	local g=Duel.GetFieldGroup(c:GetControler(),0,LOCATION_MZONE)
	local b2=Duel.GetLocationCount(1-c:GetControler(),LOCATION_MZONE)>-3 and Duel.GetTributeCount(c,g)>=3
	c:ResetFlagEffect(10000050)
	return b1 or b2
end
function c10000050.ttop(e,tp,eg,ep,ev,re,r,rp,c)
	local c=e:GetHandler()
	local b1=Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>-3 and Duel.GetTributeCount(c)>=3
	c:RegisterFlagEffect(10000050,0,0,1)
	local g=Duel.GetFieldGroup(c:GetControler(),0,LOCATION_MZONE)
	local b2=Duel.GetLocationCount(1-c:GetControler(),LOCATION_MZONE)>-3 and Duel.GetTributeCount(c,g)>=3
	local op=0
	if b1 and b2 then op=Duel.SelectOption(tp,aux.Stringid(10000050,0),aux.Stringid(10000050,1))
	elseif b1 then op=Duel.SelectOption(tp,aux.Stringid(10000050,0))
	else op=Duel.SelectOption(tp,aux.Stringid(10000050,1))+1 end
	if op==0 then
		c:ResetFlagEffect(10000050)
		local mg=Duel.SelectTribute(tp,c,3,3)
		c:SetMaterial(mg)
		Duel.Release(mg,REASON_SUMMON+REASON_MATERIAL)
	else
		local mg=Duel.SelectTribute(tp,c,3,3,g)
		c:SetMaterial(mg)
		Duel.Release(mg,REASON_SUMMON+REASON_MATERIAL)
		c:ResetFlagEffect(10000050)
		e:SetProperty(EFFECT_FLAG_SPSUM_PARAM)
		e:SetTargetRange(POS_FACEUP_ATTACK,1)
	end
end
function c10000050.setcon(e,c)
	if not c then return true end
	return false
end
function c10000050.retreg(e,tp,eg,ep,ev,re,r,rp)
	local e1=Effect.CreateEffect(e:GetHandler())
	e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_PHASE+PHASE_END)
	e1:SetRange(LOCATION_MZONE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_IGNORE_IMMUNE)
	e1:SetLabel(Duel.GetTurnCount())
	e1:SetCountLimit(1)
	e1:SetCondition(c10000050.retcon)
	e1:SetOperation(c10000050.retop)
	e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END,2)
	e:GetHandler():RegisterEffect(e1)
end
function c10000050.retcon(e,tp,eg,ep,ev,re,r,rp)
	return Duel.GetTurnCount()~=e:GetLabel()
end
function c10000050.retop(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	if c:GetControler()~=c:GetOwner() then
		Duel.GetControl(c,c:GetOwner())
	end
end
function c10000050.spcost(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return e:GetHandler():IsReleasable() end
	Duel.Release(e:GetHandler(),REASON_COST)
end
function c10000050.filter(c,e,tp)
	return c:IsCode(10000010) and c:IsCanBeSpecialSummoned(e,0,tp,true,false)
end
function c10000050.sptg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return Duel.GetLocationCount(tp,LOCATION_MZONE)>-1
		and Duel.IsExistingMatchingCard(c10000050.filter,tp,LOCATION_HAND+LOCATION_DECK,0,1,nil,e,tp) end
	Duel.SetOperationInfo(0,CATEGORY_SPECIAL_SUMMON,nil,0,tp,LOCATION_HAND+LOCATION_DECK)
end
function c10000050.spop(e,tp,eg,ep,ev,re,r,rp)
	if Duel.GetLocationCount(tp,LOCATION_MZONE)<=0 then return end
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_SPSUMMON)
	local g=Duel.SelectMatchingCard(tp,c10000050.filter,tp,LOCATION_HAND+LOCATION_DECK,0,1,1,nil,e,tp)
	local tc=g:GetFirst()
	if tc and Duel.SpecialSummonStep(tc,0,tp,tp,true,false,POS_FACEUP) then
		local e1=Effect.CreateEffect(e:GetHandler())
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_SET_ATTACK)
		e1:SetValue(4000)
		e1:SetReset(RESET_EVENT+0x1fe0000)
		tc:RegisterEffect(e1)
		local e2=e1:Clone()
		e2:SetCode(EFFECT_SET_DEFENCE)
		tc:RegisterEffect(e2)
		Duel.SpecialSummonComplete()
	end
end
