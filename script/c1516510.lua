--ルーンアイズ・ペンデュラム・ドラゴン
function c1516510.initial_effect(c)
	--fusion material
	c:EnableReviveLimit()
	aux.AddFusionProcCodeFun(c,16178681,aux.FilterBoolFunction(Card.IsRace,RACE_SPELLCASTER),1,true,false)
	--multi attack
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e1:SetCode(EVENT_SPSUMMON_SUCCESS)
	e1:SetCondition(c1516510.condition)
	e1:SetOperation(c1516510.operation)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_MATERIAL_CHECK)
	e2:SetValue(c1516510.valcheck)
	e2:SetLabelObject(e1)
	c:RegisterEffect(e2)
end
function c1516510.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_FUSION
end
function c1516510.operation(e,tp,eg,ep,ev,re,r,rp)
	local c=e:GetHandler()
	local flag=e:GetLabel()
	if bit.band(flag,0x3)~=0 then
		local e1=Effect.CreateEffect(c)
		e1:SetType(EFFECT_TYPE_SINGLE)
		e1:SetCode(EFFECT_EXTRA_ATTACK)
		e1:SetProperty(EFFECT_FLAG_CLIENT_HINT)
		if bit.band(flag,0x1)~=0 then
			e1:SetDescription(aux.Stringid(1516510,0))
			e1:SetValue(1)
		else
			e1:SetDescription(aux.Stringid(1516510,1))
			e1:SetValue(2)
		end
		e1:SetReset(RESET_EVENT+0x1fe0000)
		c:RegisterEffect(e1)
	end
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
	e2:SetCondition(c1516510.dircon)
	e2:SetReset(RESET_EVENT+0x1fe0000)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_CANNOT_ATTACK)
	e3:SetCondition(c1516510.atkcon)
	c:RegisterEffect(e3)
	if bit.band(flag,0x4)~=0 then
		local e4=Effect.CreateEffect(c)
		e4:SetType(EFFECT_TYPE_SINGLE)
		e4:SetCode(EFFECT_IMMUNE_EFFECT)
		e4:SetValue(c1516510.efilter)
		e4:SetOwnerPlayer(tp)
		e4:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+RESET_END)
		c:RegisterEffect(e4)
	end
end
function c1516510.dircon(e)
	return e:GetHandler():GetAttackAnnouncedCount()>0
end
function c1516510.atkcon(e)
	return e:GetHandler():IsDirectAttacked()
end
function c1516510.efilter(e,re)
	return e:GetOwnerPlayer()~=re:GetOwnerPlayer()
end
function c1516510.lvfilter(c)
	return c:IsCode(16178681) or c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE)
end
function c1516510.imfilter(c)
	return c:IsLocation(LOCATION_MZONE) and c:GetSummonType()==SUMMON_TYPE_PENDULUM
end
function c1516510.valcheck(e,c)
	local g=c:GetMaterial()
	local flag=0
	if g:GetCount()==2 then
		local lv=0
		local lg1=g:Filter(c1516510.lvfilter,nil)
		local lg2=g:Filter(Card.IsRace,nil,RACE_SPELLCASTER)
		if lg1:GetCount()==2 then
			lv=lg2:GetFirst():GetOriginalLevel()
			local lc=lg2:GetNext()
			if lc then lv=math.max(lv,lc:GetOriginalLevel()) end
		else
			local lc=g:GetFirst()
			if lc==lg1:GetFirst() then lc=g:GetNext() end
			lv=lc:GetOriginalLevel()
		end
		if lv>4 then
			flag=0x2
		elseif lv>0 then
			flag=0x1
		end
	end
	if g:IsExists(c1516510.imfilter,1,nil) then
		flag=flag+0x4
	end
	e:GetLabelObject():SetLabel(flag)
end
