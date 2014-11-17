--ルーンアイズ・ペンデュラム・ドラゴン
function c1516510.initial_effect(c)
	c:EnableReviveLimit()
	--fusion material
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e1:SetCode(EFFECT_FUSION_MATERIAL)
	e1:SetCondition(c1516510.fscon)
	e1:SetOperation(c1516510.fsop)
	c:RegisterEffect(e1)
end
c1516510.material_count=1
c1516510.material={16178681}
function c1516510.mfilter(c,mg)
	return (c:IsCode(16178681) or c:IsHasEffect(EFFECT_FUSION_SUBSTITUTE)) and mg:IsExists(Card.IsRace,1,c,RACE_SPELLCASTER)
end
function c1516510.fscon(e,mg,gc)
	if mg==nil then return false end
	if gc then return false end
	return mg:IsExists(c1516510.mfilter,1,nil,mg)
end
function c1516510.imfilter(c)
	return c:IsLocation(LOCATION_MZONE) and c:GetSummonType()==SUMMON_TYPE_PENDULUM
end
function c1516510.fsop(e,tp,eg,ep,ev,re,r,rp,gc)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g1=eg:FilterSelect(tp,c1516510.mfilter,1,1,nil,eg)
	Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_FMATERIAL)
	local g2=eg:FilterSelect(tp,Card.IsRace,1,1,g1:GetFirst(),RACE_SPELLCASTER)
	g1:Merge(g2)
	Duel.SetFusionMaterial(g1)
	local c=e:GetHandler()
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_EXTRA_ATTACK)
	e1:SetProperty(EFFECT_FLAG_CLIENT_HINT)
	if g2:GetFirst():GetOriginalLevel()<5 then
		e1:SetDescription(aux.Stringid(1516510,0))
		e1:SetValue(1)
	else
		e1:SetDescription(aux.Stringid(1516510,1))
		e1:SetValue(2)
	end
	e1:SetReset(RESET_EVENT+0xfe0000)
	c:RegisterEffect(e1)
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetCode(EFFECT_CANNOT_DIRECT_ATTACK)
	e2:SetCondition(c1516510.dircon)
	e2:SetReset(RESET_EVENT+0xfe0000)
	c:RegisterEffect(e2)
	local e3=e2:Clone()
	e3:SetCode(EFFECT_CANNOT_ATTACK)
	e3:SetCondition(c1516510.atkcon)
	c:RegisterEffect(e3)
	if g1:IsExists(c1516510.imfilter,1,nil) then
		local e4=Effect.CreateEffect(c)
		e4:SetType(EFFECT_TYPE_SINGLE)
		e4:SetCode(EFFECT_IMMUNE_EFFECT)
		e4:SetValue(c1516510.efilter)
		e4:SetReset(RESET_EVENT+0xfe0000+RESET_PHASE+RESET_END)
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
	return e:GetHandlerPlayer()~=re:GetHandlerPlayer()
end
