--一回休み
function c24348804.initial_effect(c)
	--Activate
  local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(0,TIMING_SUMMON+TIMING_SPSUMMON)
	e1:SetCondition(c24348804.condition)
	e1:SetTarget(c24348804.postg0)
	e1:SetOperation(c24348804.posop)
	c:RegisterEffect(e1)
    --special summon
    local e2=Effect.CreateEffect(c)
    e2:SetType(EFFECT_TYPE_FIELD)
    e2:SetCode(EFFECT_DISABLE)
    e2:SetRange(LOCATION_SZONE)
    e2:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
    e2:SetTarget(c24348804.target)
    c:RegisterEffect(e2)
	if not c24348804.global_check then
		c24348804.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge1:SetOperation(c24348804.checkop)
		Duel.RegisterEffect(ge1,0)
	end
	--pos
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(24348804,1))
	e3:SetCategory(CATEGORY_POSITION)
	e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e3:SetCode(EVENT_SPSUMMON_SUCCESS)
	e3:SetRange(LOCATION_SZONE)
	e3:SetLabel(1)
	e3:SetTarget(c24348804.postg)
	e3:SetOperation(c24348804.posop)
	c:RegisterEffect(e3)
end
function c24348804.cfilter(c)
	return bit.band(c:GetSummonType(),SUMMON_TYPE_SPECIAL)==SUMMON_TYPE_SPECIAL
end
function c24348804.postg0(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	local res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_SPSUMMON_SUCCESS,true)
	if res then
		local g=teg:Filter(c24348804.filter,nil)
		if g:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(24348804,0)) then
			e:SetLabel(1)
			Duel.SetTargetCard(g)
			Duel.SetOperationInfo(0,CATEGORY_DISABLE,g,g:GetCount(),0,0)
			e:GetHandler():RegisterFlagEffect(0,RESET_CHAIN,EFFECT_FLAG_CLIENT_HINT,1,0,aux.Stringid(24348804,2))
		end
	end
end
function c24348804.condition(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(c88197162.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c24348804.target(e,c)
    return c:GetFlagEffect(24348804)>0
end
function c24348804.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		if tc:GetFlagEffect(24348804)==0 then
			tc:RegisterFlagEffect(24348804,RESET_PHASE+PHASE_END,0,1)
		end
		tc=eg:GetNext()
	end
end
function c24348804.filter(c,e)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsType(TYPE_EFFECT) and (not e or c:IsRelateToEffect(e))
end
function c24348804.postg(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c24348804.filter,1,nil) end
	Duel.SetTargetCard(eg)
end
function c24348804.posop(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	local g=eg:Filter(c24348804.filter,nil,e)
	Duel.ChangePosition(g,POS_FACEUP_DEFENCE)
end
