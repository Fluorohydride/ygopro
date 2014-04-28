--古代の機械合成獣
function c86321248.initial_effect(c)
	--mat check
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_MATERIAL_CHECK)
	e1:SetValue(c86321248.valcheck)
	c:RegisterEffect(e1)
	--summon success
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetCondition(c86321248.regcon)
	e2:SetOperation(c86321248.regop)
	c:RegisterEffect(e2)
	e2:SetLabelObject(e1)
end
function c86321248.valcheck(e,c)
	local g=c:GetMaterial()
	local flag=0
	local tc=g:GetFirst()
	while tc do
		local code=tc:GetCode()
		if code==41172955 then flag=bit.bor(flag,0x1)
		elseif code==86445415 then flag=bit.bor(flag,0x2)
		elseif code==13839120 then flag=bit.bor(flag,0x4)
		end
		tc=g:GetNext()
	end
	e:SetLabel(flag)
end
function c86321248.regcon(e,tp,eg,ep,ev,re,r,rp)
	return bit.band(e:GetHandler():GetSummonType(),SUMMON_TYPE_ADVANCE)==SUMMON_TYPE_ADVANCE
end
function c86321248.regop(e,tp,eg,ep,ev,re,r,rp)
	local flag=e:GetLabelObject():GetLabel()
	local c=e:GetHandler()
	if bit.band(flag,0x1)~=0 then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_UPDATE_ATTACK)
		e1:SetValue(300)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
	end
	if bit.band(flag,0x2)~=0 then
		local e1=Effect.CreateEffect(c)
		e1:SetDescription(aux.Stringid(86321248,0))
		e1:SetCategory(CATEGORY_DAMAGE)
		e1:SetProperty(EFFECT_FLAG_PLAYER_TARGET)
		e1:SetCode(EVENT_BATTLE_DAMAGE)
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
		e1:SetCondition(c86321248.damcon1)
		e1:SetTarget(c86321248.damtg1)
		e1:SetOperation(c86321248.damop)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
	end
	if bit.band(flag,0x4)~=0 then
		local e1=Effect.CreateEffect(c)
		e1:SetDescription(aux.Stringid(86321248,1))
		e1:SetCategory(CATEGORY_DAMAGE)
		e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
		e1:SetCode(EVENT_BATTLE_DESTROYING)
		e1:SetTarget(c86321248.damtg2)
		e1:SetOperation(c86321248.damop)
		e1:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e1)
	end
end
function c86321248.damcon1(e,tp,eg,ep,ev,re,r,rp)
	return ep~=tp and Duel.GetAttackTarget()==nil
end
function c86321248.damtg1(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(500)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,500)
end
function c86321248.damtg2(e,tp,eg,ep,ev,re,r,rp,chk)
	if chk==0 then return true end
	Duel.SetTargetPlayer(1-tp)
	Duel.SetTargetParam(700)
	Duel.SetOperationInfo(0,CATEGORY_DAMAGE,nil,0,1-tp,700)
end
function c86321248.damop(e,tp,eg,ep,ev,re,r,rp)
	local p,d=Duel.GetChainInfo(0,CHAININFO_TARGET_PLAYER,CHAININFO_TARGET_PARAM)
	Duel.Damage(p,d,REASON_EFFECT)
end
