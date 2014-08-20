--雲魔物－ニンバスマン
function c20003527.initial_effect(c)
	--battle indestructable
	local e1=Effect.CreateEffect(c)
	e1:SetType(EFFECT_TYPE_SINGLE)
	e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
	e1:SetValue(1)
	c:RegisterEffect(e1)
	--selfdes
	local e2=Effect.CreateEffect(c)
	e2:SetType(EFFECT_TYPE_SINGLE)
	e2:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e2:SetRange(LOCATION_MZONE)
	e2:SetCode(EFFECT_SELF_DESTROY)
	e2:SetCondition(c20003527.sdcon)
	c:RegisterEffect(e2)
	--summon proc
	local e3=Effect.CreateEffect(c)
	e3:SetDescription(aux.Stringid(20003527,0))
	e3:SetProperty(EFFECT_FLAG_CANNOT_DISABLE+EFFECT_FLAG_UNCOPYABLE)
	e3:SetType(EFFECT_TYPE_SINGLE)
	e3:SetCode(EFFECT_SUMMON_PROC)
	e3:SetCondition(c20003527.sumcon)
	e3:SetOperation(c20003527.sumop)
	e3:SetValue(SUMMON_TYPE_ADVANCE)
	c:RegisterEffect(e3)
	--tribute check
	local e4=Effect.CreateEffect(c)
	e4:SetType(EFFECT_TYPE_SINGLE)
	e4:SetCode(EFFECT_MATERIAL_CHECK)
	e4:SetValue(c20003527.valcheck)
	c:RegisterEffect(e4)
	--add counter
	local e5=Effect.CreateEffect(c)
	e5:SetDescription(aux.Stringid(20003527,0))
	e5:SetCategory(CATEGORY_COUNTER)
	e5:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_F)
	e5:SetCode(EVENT_SUMMON_SUCCESS)
	e5:SetCondition(c20003527.addcon)
	e5:SetOperation(c20003527.addc)
	e5:SetLabelObject(e4)
	c:RegisterEffect(e5)
	--atk
	local e6=Effect.CreateEffect(c)
	e6:SetType(EFFECT_TYPE_SINGLE)
	e6:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
	e6:SetRange(LOCATION_MZONE)
	e6:SetCode(EFFECT_UPDATE_ATTACK)
	e6:SetValue(c20003527.atkval)
	c:RegisterEffect(e6)
end
function c20003527.sdcon(e)
	return e:GetHandler():IsPosition(POS_FACEUP_DEFENCE)
end
function c20003527.cfilter(c,tp)
	return c:IsAttribute(ATTRIBUTE_WATER) and (c:IsControler(tp) or c:IsFaceup())
end
function c20003527.sumcon(e,c)
	if c==nil then return true end
	local tp=c:GetControler()
	local mg=Duel.GetMatchingGroup(c20003527.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	return c:GetLevel()>4 and Duel.GetTributeCount(c,mg)>0
end
function c20003527.sumop(e,tp,eg,ep,ev,re,r,rp,c)
	local mg=Duel.GetMatchingGroup(c20003527.cfilter,tp,LOCATION_MZONE,LOCATION_MZONE,nil,tp)
	local sg=Duel.SelectTribute(tp,c,1,10,mg)
	c:SetMaterial(sg)
	Duel.Release(sg,REASON_SUMMON+REASON_MATERIAL)
end
function c20003527.valcheck(e,c)
	local g=c:GetMaterial()
	e:SetLabel(g:FilterCount(Card.IsAttribute,nil,ATTRIBUTE_WATER))
end
function c20003527.addcon(e,tp,eg,ep,ev,re,r,rp)
	return e:GetHandler():GetSummonType()==SUMMON_TYPE_ADVANCE
end
function c20003527.addc(e,tp,eg,ep,ev,re,r,rp)
	if e:GetHandler():IsRelateToEffect(e) then
		e:GetHandler():AddCounter(0x19,e:GetLabelObject():GetLabel())
	end
end
function c20003527.atkval(e,c)
	return Duel.GetCounter(0,1,1,0x19)*500
end
