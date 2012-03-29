--天魔神 インヴィシル
function c74841885.initial_effect(c)
	--cannot special summon
	local e1=Effect.CreateEffect(c)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_SPSUMMON_CONDITION)
	c:RegisterEffect(e1)
	--summon success
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_SUMMON_SUCCESS)
	e2:SetCondition(c74841885.condition)
	e2:SetOperation(c74841885.operation)
	c:RegisterEffect(e2)
	--tribute check
	local e3=Effect.CreateEffect(c)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_MATERIAL_CHECK)
	e3:SetValue(c74841885.valcheck)
	e3:SetLabelObject(e2)
	c:RegisterEffect(e3)
end
function c74841885.chkfilter(c,rac,att)
	return c:IsRace(rac) and c:IsAttribute(att)
end
function c74841885.valcheck(e,c)
	local g=c:GetMaterial()
	local lbl=0
	if g:IsExists(c74841885.chkfilter,1,nil,RACE_FAIRY,ATTRIBUTE_LIGHT) then
		lbl=lbl+TYPE_SPELL
	end
	if g:IsExists(c74841885.chkfilter,1,nil,RACE_FIEND,ATTRIBUTE_DARK) then
		lbl=lbl+TYPE_TRAP
	end
	e:GetLabelObject():SetLabel(lbl)
end
function c74841885.condition(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_ADVANCE and e:GetLabel()~=0
end
function c74841885.operation(e,tp,eg,ep,ev,re,r,rp)
	--disable
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	if e:GetLabel()==TYPE_SPELL then
		e1:SetDescription(aux.Stringid(74841885,0))
	elseif e:GetLabel()==TYPE_TRAP then
		e1:SetDescription(aux.Stringid(74841885,1))
	else
		e1:SetDescription(aux.Stringid(74841885,2))
	end
	e1:SetProperty(EFFECT_FLAG_CLIENT_HINT)
	e1:SetType(EFFECT_TYPE_FIELD)
	e1:SetCode(EFFECT_DISABLE)
	e1:SetRange(LOCATION_MZONE)
	e1:SetTargetRange(LOCATION_SZONE,LOCATION_SZONE)
	e1:SetTarget(c74841885.distg)
	e1:SetLabel(e:GetLabel())
	e1:SetReset(RESET_EVENT+0x1ff0000)
	c:RegisterEffect(e1)
	--disable effect
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
	e2:SetCode(EVENT_CHAIN_SOLVING)
	e2:SetRange(LOCATION_MZONE)
	e2:SetLabel(e:GetLabel())
	e2:SetOperation(c74841885.disop)
	e2:SetReset(RESET_EVENT+0x1ff0000)
	c:RegisterEffect(e2)
	if bit.band(e:GetLabel(),TYPE_TRAP)~=0 then
		local e3=Effect.CreateEffect(c)
		e3:SetType(EFFECT_TYPE_FIELD)
		e3:SetCode(EFFECT_DISABLE_TRAPMONSTER)
		e3:SetRange(LOCATION_MZONE)
		e3:SetTargetRange(LOCATION_MZONE,LOCATION_MZONE)
		e3:SetTarget(c74841885.distg)
		e3:SetLabel(TYPE_TRAP)
		e3:SetReset(RESET_EVENT+0x1ff0000)
		c:RegisterEffect(e3)
	end
end
function c74841885.distg(e,c)
	return c:IsType(e:GetLabel())
end
function c74841885.disop(e,tp,eg,ep,ev,re,r,rp)
	local tl=Duel.GetChainInfo(ev,CHAININFO_TRIGGERING_LOCATION)
	if tl==LOCATION_SZONE and re:IsActiveType(e:GetLabel()) then
		Duel.NegateEffect(ev)
	end
end
