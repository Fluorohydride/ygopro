--一回休み
function c24348804.initial_effect(c)
	--Activate
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_ACTIVATE)
	e1:SetCode(EVENT_FREE_CHAIN)
	e1:SetHintTiming(TIMING_SPSUMMON)
	e1:SetCondition(c24348804.condition)
	e1:SetTarget(c24348804.target1)
	e1:SetOperation(c24348804.operation)
	c:RegisterEffect(e1)
	--pos
	local e2=Effect.CreateEffect(c)
	e2:SetCategory(CATEGORY_POSITION)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_F)
	e2:SetCode(EVENT_SPSUMMON_SUCCESS)
	e2:SetRange(LOCATION_SZONE)
	e2:SetTarget(c24348804.target2)
	e2:SetOperation(c24348804.operation)
	e2:SetLabel(1)
	c:RegisterEffect(e2)
	--disable
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_FIELD)
	e3:SetCode(EFFECT_DISABLE)
	e3:SetRange(LOCATION_SZONE)
	e3:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
	e3:SetTarget(c24348804.distg)
	c:RegisterEffect(e3)
	if not c24348804.global_check then
		c24348804.global_check=true
		local ge1=Effect.CreateEffect(c)
		ge1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
		ge1:SetCode(EVENT_SPSUMMON_SUCCESS)
		ge1:SetOperation(c24348804.checkop)
		Duel.RegisterEffect(ge1,0)
	end
end
function c24348804.checkop(e,tp,eg,ep,ev,re,r,rp)
	local tc=eg:GetFirst()
	while tc do
		tc:RegisterFlagEffect(24348804,RESET_EVENT+0x1ec0000+RESET_PHASE+PHASE_END,0,1)
		tc=eg:GetNext()
	end
end
function c24348804.cfilter(c)
	return bit.band(c:GetSummonType(),SUMMON_TYPE_SPECIAL)==SUMMON_TYPE_SPECIAL
end
function c24348804.condition(e,tp,eg,ep,ev,re,r,rp)
	return not Duel.IsExistingMatchingCard(c24348804.cfilter,tp,LOCATION_MZONE,0,1,nil)
end
function c24348804.target1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	e:SetLabel(0)
	local res,teg,tep,tev,tre,tr,trp=Duel.CheckEvent(EVENT_SPSUMMON_SUCCESS,true)
	if res then
		local g=teg:Filter(c24348804.filter1,nil)
		if g:GetCount()>0 and Duel.SelectYesNo(tp,aux.Stringid(24348804,0)) then
			Duel.SetTargetCard(g)
			Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
			e:SetLabel(1)
			e:GetHandler():RegisterFlagEffect(0,RESET_CHAIN,EFFECT_FLAG_CLIENT_HINT,1,0,aux.Stringid(24348804,1))
		end
	end
end
function c24348804.filter1(c)
	return c:IsPosition(POS_FACEUP_ATTACK) and c:IsType(TYPE_EFFECT)
end
function c24348804.target2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return eg:IsExists(c24348804.filter1,1,nil) end
	local g=eg:Filter(c24348804.filter1,nil)
	Duel.SetTargetCard(g)
	Duel.SetOperationInfo(0,CATEGORY_POSITION,g,g:GetCount(),0,0)
end
function c24348804.operation(e,tp,eg,ep,ev,re,r,rp)
	if e:GetLabel()==0 or not e:GetHandler():IsRelateToEffect(e) then return end
	local g=Duel.GetChainInfo(0,CHAININFO_TARGET_CARDS):Filter(Card.IsRelateToEffect,nil,e)
	if g:GetCount()>0 then
		Duel.ChangePosition(g,POS_FACEUP_DEFENCE,POS_FACEDOWN_DEFENCE,POS_FACEUP_DEFENCE,POS_FACEDOWN_DEFENCE)
	end
end
function c24348804.distg(e,c)
	return c:GetFlagEffect(24348804)~=0
end
